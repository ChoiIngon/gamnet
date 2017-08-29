/*
 * Test.h
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_TEST_TESTER_H_
#define GAMNET_TEST_TESTER_H_

#include <map>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include "../Network/Tcp/LinkManager.h"
#include "../Library/ThreadPool.h"

namespace Gamnet { namespace Test {
template <class SESSION_T>
class LinkManager : public Network::Tcp::LinkManager<SESSION_T>
{
public :
	typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
	typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
private:
	struct TestExecuteInfo
	{
		std::string name;
		std::atomic_int execute_count;

		int send_id;
		int recv_id;
		SEND_HANDLER_TYPE send_handler;
		RECV_HANDLER_TYPE recv_handler;
	};


	std::mutex lock_;
	std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
	std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;
	std::map<unsigned int, RECV_HANDLER_TYPE> 				recv_handler;

	Log::Logger	log;
	Timer 		log_timer;
	Timer 		execute_timer;
	ThreadPool 	thread_pool;
	int 		execute_count;
	
	std::string host;
	int			port;
public :
	LinkManager() : thread_pool(30), execute_count(0), host(""), port(0)
	{
		std::shared_ptr<TestExecuteInfo> info = std::shared_ptr<TestExecuteInfo>(new TestExecuteInfo());
		info->name = "__connect__";
		info->send_handler = std::bind(&LinkManager<SESSION_T>::Send_Connect_Req, this, std::placeholders::_1);
		info->recv_handler = std::bind(&LinkManager<SESSION_T>::Recv_Connect_Ans, this, std::placeholders::_1, std::placeholders::_2);
		execute_infos[info->name] = info;
		recv_handler[Network::Tcp::LinkManager<SESSION_T>::MsgHandler::MsgID_Connect_Ans] = info->recv_handler;
		SetTestSequence(info->name);
	}
	virtual ~LinkManager()
	{
	}

	void Init(const char* host, int port, int interval, int session_count, int execute_count)
	{
		log.Init("test", "test", 5);
		if(0 == interval)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::InvalidArgumentError), " 'interval' should be set");
		}
		if(0 == session_count)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::InvalidArgumentError), " 'session_count' should be set");
		}

		this->host = host;
		this->port = port;

		LOG(GAMNET_INF, "test start...");
		execute_timer.SetTimer(interval, [this, session_count, execute_count]() {
			for(size_t i=0; i<session_count; i++)
			{
				if(session_count <= this->session_pool.Capacity() - this->session_pool.Available())
				{
					break;
				}
				thread_pool.PostTask([this]() {
					std::shared_ptr<Network::Link> link = this->Create();
					if(NULL == link) 
					{
						LOG(ERR, "can not create any more link");
						return;
					}
					link->AttachManager(this);

					std::shared_ptr<SESSION_T> session = this->session_pool.Create();
					if(NULL == session)
					{
						LOG(ERR, GAMNET_ERRNO(ErrorCode::CreateInstanceFailError), "can not create any more session(max:", this->session_pool.Capacity(), ", current:", this->session_pool.Available(), ")");
						return;
					}
					session->session_key = ++Network::SessionManager::session_key;
					session->recv_packet = Network::Tcp::Packet::Create();
					session->msg_seq = 0;
					session->test_seq = 0;

					link->AttachSession(session);
					link->Connect(this->host.c_str(), this->port, 30);
				});

				this->execute_count++;
				if(this->execute_count >= execute_count)
				{
					break;
				}
			}

			if(this->execute_count < execute_count)
			{
				execute_timer.Resume();
			}
		});

		log_timer.SetTimer(3000, [this, execute_count]() {
			log.Write(GAMNET_INF, "[Test] execute count : ", this->execute_count);
			log.Write(GAMNET_INF, "[Test] running session : ", this->session_manager.Size(), ", idle session : ", this->session_pool.Available());
			for(auto itr : execute_order)
			{
				log.Write(GAMNET_INF, "[Test] running state(name : ", itr->name, ", count : ", itr->execute_count, ")");
			}
			if(this->execute_count < execute_count)
			{
				log_timer.Resume();
			}
			else
			{
				log.Write(GAMNET_INF, "[Test] test finished..(", this->execute_count, "/", execute_count, ")");
			}
		});
	}

	virtual void OnConnect(const std::shared_ptr<Network::Link>& link)
	{
		const std::shared_ptr<SESSION_T>& session = std::static_pointer_cast<SESSION_T>(link->session);
		if(session->test_seq < (int)execute_order.size())
		{
			execute_order[session->test_seq]->send_handler(session);
			execute_order[session->test_seq]->execute_count++;
		}
	}

	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason)
	{
		Network::LinkManager::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if(NULL == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
			link->OnError(EINVAL);
			return;
		}

		session->recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while(Network::Tcp::Packet::HEADER_SIZE <= (int)session->recv_packet->Size())
		{
			uint16_t totalLength = session->recv_packet->GetLength();
			if(Network::Tcp::Packet::HEADER_SIZE > totalLength )
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			if(totalLength >= session->recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			// 데이터가 부족한 경우
			if(totalLength > (uint16_t)session->recv_packet->Size())
			{
				break;
			}

			uint32_t msg_id = session->recv_packet->GetID();
			auto itr = recv_handler.find(msg_id);
			if(itr == recv_handler.end())
			{
				LOG(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
				link->OnError(0);
				return ;
			}
			RECV_HANDLER_TYPE& handler = itr->second;
			handler(session, session->recv_packet);

			session->test_seq++;
			if(session->test_seq >= (int)execute_order.size())
			{
				link->OnError(0);
				return;
			}

			const std::shared_ptr<TestExecuteInfo>& execute_info = execute_order[session->test_seq];
			try
			{
				execute_info->send_handler(session);
			}
			catch(const Gamnet::Exception& e)
			{
				LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
				link->OnError(0);
				return;
			}
			execute_info->execute_count++;
			uint16_t length = session->recv_packet->GetLength();
			session->recv_packet->Remove(length);

			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if(NULL == packet)
			{
				LOG(GAMNET_ERR, "Can't create more buffer(link_key:", link->link_key, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			if(0 < session->recv_packet->Size())
			{
				packet->Append(session->recv_packet->ReadPtr(), session->recv_packet->Size());
			}

			session->recv_packet = packet;
		}
	}

	template<class NTF_T>
	void RegisterHandler(RECV_HANDLER_TYPE recv)
	{
		recv_handler[NTF_T::MSG_ID] = recv;
	}
	template<class REQ_T, class ANS_T>
	void RegisterHandler(const std::string& test_name, SEND_HANDLER_TYPE send, RECV_HANDLER_TYPE recv)
	{
		std::shared_ptr<TestExecuteInfo> info = std::shared_ptr<TestExecuteInfo>(new TestExecuteInfo());
		info->name = test_name;
		info->send_handler = send;
		info->recv_handler = recv;
		execute_infos[test_name] = info;
		RegisterHandler<ANS_T>(recv);
	}
	void SetTestSequence(const std::string& test_name)
	{
		auto itr = execute_infos.find(test_name);
		if(execute_infos.end() == itr)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] can't find registered test case(test_name:", test_name, ")");
		}
		const std::shared_ptr<TestExecuteInfo>& info = itr->second;
		execute_order.push_back(info);
	}

	void Send_Connect_Req(const std::shared_ptr<SESSION_T>& session)
	{
		Network::Tcp::Packet::Header header;
		header.msg_id = Network::Tcp::LinkManager<SESSION_T>::MsgHandler::MsgID_Connect_Req;
		header.msg_seq = ++session->msg_seq;
		header.length = Network::Tcp::Packet::HEADER_SIZE;

		std::shared_ptr<Network::Tcp::Packet> req_packet = Network::Tcp::Packet::Create();
		req_packet->Write(header, NULL, 0);

		session->Send(req_packet);
	}

	void Recv_Connect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
		Json::Value ans;
		Json::Reader reader;
		if (false == reader.parse(json, ans))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] parse error(msg:", json, ")");
		}

		if(ErrorCode::Success == ans["error_code"].asInt())
		{
			session->session_token = ans["session_token"].asString();
		}
	}
};

}} /* namespace Gamnet */

#endif /* TEST_H_ */
