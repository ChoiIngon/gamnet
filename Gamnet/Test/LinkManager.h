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
	GAMNET_WHERE(SESSION_T, Session);
public :
	typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
	typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
	
private:
	struct RecvHandlerInfo
	{
		uint32_t msg_id;		
		int increase_test_seq; // 1
		RECV_HANDLER_TYPE recv_handler;
	};
	
	struct TestExecuteInfo
	{
		TestExecuteInfo()
		{
			name = "";
			execute_count = 0;
		}
		std::string name;
		std::atomic_uint execute_count;
		SEND_HANDLER_TYPE send_handler;		
		std::map<uint32_t, std::shared_ptr<RecvHandlerInfo>> recv_handlers;
	};

	std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
	std::map<std::string, std::list<std::shared_ptr<RecvHandlerInfo>>> 	recv_handlers;

	Log::Logger	log;
	Timer 		log_timer;
	Timer 		execute_timer;
	ThreadPool 	thread_pool;
	std::atomic_int	execute_count;
	
	std::string host;
	int			port;
public :
	std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;
	LinkManager() : thread_pool(30), execute_count(0), host(""), port(0)
	{
		Network::Tcp::LinkManager<SESSION_T>::_name = "Gamnet::Test::LinkManager";

		RegisterSendHandler("__connect__", std::bind(&LinkManager<SESSION_T>::Send_Connect_Req, this, std::placeholders::_1));
		RegisterRecvHandler("__connect__", (uint32_t)Network::Tcp::LinkManager<SESSION_T>::MsgID_SvrCli_Connect_Ans, std::bind(&LinkManager<SESSION_T>::Recv_Connect_Ans, this, std::placeholders::_1, std::placeholders::_2), 1);
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
				if(this->Size() >= session_count || 0 == this->Available())
				{
					break;
				}
				std::shared_ptr<Network::Link> link = this->Create();
				if (nullptr == link)
				{
					LOG(GAMNET_ERR, "can not create link(link_count:", this->Size(), ", avaiable:", this->Available(), ", capacity:", this->Capacity(), ")");
					return;
				}

				std::shared_ptr<SESSION_T> session = this->session_pool.Create();
				if (nullptr == session)
				{
					LOG(GAMNET_ERR, "can not create session(max:", this->session_pool.Capacity(), ", current:", this->session_pool.Available(), ")");
					link->Close(ErrorCode::InvalidSessionError);
					return;
				}
				
				session->test_seq = 0;

				link->AttachSession(session);
				session->OnCreate();
				thread_pool.PostTask([this, link]() {
					link->Connect(this->host.c_str(), this->port, 5);
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
			log.Write(GAMNET_INF, "[Test] execute count : ", this->execute_count, "/", execute_count);
			log.Write(GAMNET_INF, "[Test] link count : (active:", this->Size(), ", available : ", this->Available(), ", max:", this->Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] session count : (active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");
			
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
		thread_pool.PostTask([this, link]() {
			const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if(0 == session->test_seq && 0 < (int)this->execute_order.size())
			{
				const std::shared_ptr<TestExecuteInfo>& info = execute_order[0];
				info->send_handler(session);
				info->execute_count++;
			}
		});
	}

	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason)
	{
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if(nullptr != session)
		{
			Send_Close_Ntf(session);
			session->OnDestroy();
		}
		Network::Tcp::LinkManager<SESSION_T>::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);
		const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
		if (nullptr == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
			link->Close(ErrorCode::InvalidSessionError);
			return;
		}
		
		if(session->test_seq < (int)this->execute_order.size())
		{
			uint32_t msg_id = packet->GetID();

			const std::shared_ptr<TestExecuteInfo>& execute_info = this->execute_order[session->test_seq];
			auto itr = execute_info->recv_handlers.find(msg_id);
			if(execute_info->recv_handlers.end() == itr)
			{
				itr = this->execute_order[0]->recv_handlers.find(msg_id);
				if (this->execute_order[0]->recv_handlers.end() == itr)
				{
					LOG(GAMNET_WRN, "can't find handler function(msg_id:", msg_id, ")");
					link->Close(ErrorCode::InvalidHandlerError);
					return ;
				}
			}
				
			RECV_HANDLER_TYPE& handler = itr->second->recv_handler;
			handler(session, packet);
				
			if(false == session->is_pause && 0 < itr->second->increase_test_seq)
			{
				session->test_seq += itr->second->increase_test_seq;
				if(session->test_seq >= (int)this->execute_order.size())
				{
					link->Close(ErrorCode::Success);
					return;
				}

				const std::shared_ptr<TestExecuteInfo>& next_execute_info = this->execute_order[session->test_seq];
				try
				{ 
					thread_pool.PostTask([next_execute_info, session]() {
						next_execute_info->send_handler(session);
					});
				}
				catch(const Gamnet::Exception& e)
				{
					LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
					link->Close(ErrorCode::UndefinedError);
					return;
				}
				next_execute_info->execute_count++;
			}
		}
	}

	void RegisterSendHandler(const std::string& test_name, SEND_HANDLER_TYPE send)
	{
		std::shared_ptr<TestExecuteInfo> executeInfo = std::shared_ptr<TestExecuteInfo>(new TestExecuteInfo());
		executeInfo->name = test_name;
		executeInfo->send_handler = send;
		if (false == execute_infos.insert(std::make_pair(test_name, executeInfo)).second)
		{
			Exception(GAMNET_ERRNO(ErrorCode::InvalidKeyError), "(duplicate test case name(", test_name, ")");
		}
	}

	void RegisterRecvHandler(const std::string& test_name, uint32_t msgID, RECV_HANDLER_TYPE recv, int increaseTestSEQ = 1)
	{
		std::shared_ptr<RecvHandlerInfo> recvHandlerInfo = std::shared_ptr<RecvHandlerInfo>(new RecvHandlerInfo());
		recvHandlerInfo->msg_id = msgID;
		recvHandlerInfo->increase_test_seq = increaseTestSEQ;
		recvHandlerInfo->recv_handler = recv;
		std::list<std::shared_ptr<RecvHandlerInfo>>& lstRecvHandlerInfos = recv_handlers[test_name];
		lstRecvHandlerInfos.push_back(recvHandlerInfo);
	}
	
	void SetTestSequence(const std::string& test_name)
	{
		auto itr_execute_info = execute_infos.find(test_name);
		if(execute_infos.end() == itr_execute_info)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::InvalidKeyError),"can't find registered test case execute info(test_name:", test_name, ")");
		}
		const std::shared_ptr<TestExecuteInfo>& info = itr_execute_info->second;

		auto itr_recv_handlers = recv_handlers.find(test_name);
		if(recv_handlers.end() == itr_recv_handlers)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::InvalidKeyError),"can't find registered test case recv handler(test_name:", test_name, ")");
		}
		
		for(const std::shared_ptr<RecvHandlerInfo> recv_handler : itr_recv_handlers->second)
		{
			info->recv_handlers[recv_handler->msg_id]  = recv_handler;
		}
		execute_order.push_back(info);
	}

	void Send_Connect_Req(const std::shared_ptr<SESSION_T>& session)
	{
		Network::Tcp::Packet::Header header;
		header.msg_id = Network::Tcp::LinkManager<SESSION_T>::MsgID_CliSvr_Connect_Req;
		header.msg_seq = ++session->msg_seq;
		header.length = Network::Tcp::Packet::HEADER_SIZE;

		std::shared_ptr<Network::Tcp::Packet> req_packet = Network::Tcp::Packet::Create();
		if(nullptr == req_packet)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::CreateInstanceFailError), "can not create packet");
		}
		req_packet->Write(header, nullptr, 0);

		session->AsyncSend(req_packet);
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
		session->OnConnect();
	}

	void Send_Close_Ntf(const std::shared_ptr<SESSION_T>& session)
	{
		if(nullptr == session)
		{
			LOG(GAMNET_ERR, GAMNET_ERRSTR(ErrorCode::InvalidSessionError));
			return;
		}
		Network::Tcp::Packet::Header header;
		header.msg_id = Network::Tcp::LinkManager<SESSION_T>::MsgID_CliSvr_Close_Ntf;
		header.msg_seq = ++session->msg_seq;
		header.length = Network::Tcp::Packet::HEADER_SIZE;

		std::shared_ptr<Network::Tcp::Packet> req_packet = Network::Tcp::Packet::Create();
		if (nullptr == req_packet)
		{
			LOG(GAMNET_ERR, "can not create packet");
			return;
		}
		req_packet->Write(header, nullptr, 0);
		session->AsyncSend(req_packet);
	}
};

}} /* namespace Gamnet */

#endif /* TEST_H_ */
