/*
 * Test.h
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_TEST_TESTER_H_
#define GAMNET_TEST_TESTER_H_

#include <vector>
#include <map>
#include <memory>
#include "../Network/Network.h"
#include "../Library/Timer.h"
#include "../Library/ThreadPool.h"
#include <atomic>

namespace Gamnet { namespace Test {
template <class SESSION_T>
class Tester : public Network::IListener
{
public :
	typedef std::function<void(std::shared_ptr<SESSION_T>)> SEND_HANDER_TYPE;
	typedef std::function<void(std::shared_ptr<SESSION_T>, std::shared_ptr<Network::Packet>)> RECV_HANDLER_TYPE;
private:
	struct TestRunningState
	{
		std::mutex lock_;
		std::string name_;
		unsigned long int count_;
		TestRunningState() : name_(""), count_(0) {}
		TestRunningState(const TestRunningState& t)
		{
			name_ = t.name_;
			count_ = t.count_;
		}
	};
	std::mutex lock_;
	std::vector<SEND_HANDER_TYPE> vecSendHandler_;
	std::vector<TestRunningState> vecTestRunningState_;
	std::map<std::string, SEND_HANDER_TYPE>	mapSendHandler_;
	std::map<unsigned int, RECV_HANDLER_TYPE> mapRecvHandler_;

	Pool<SESSION_T, std::mutex, Network::Session::Init> sessionPool_;
	Timer timer_;
	Timer printTimer_;
	ThreadPool threadPool_;
	Log::Logger log_;
	boost::asio::ip::tcp::endpoint endpoint_;

	uint64_t executeCount_;
	uint32_t interval_;
	uint32_t sessionCount_;
	uint32_t loopCount_;

public :
	Tester() :
		sessionPool_(1024),
		threadPool_(30),
		executeCount_(0),
		interval_(0),
		sessionCount_(0),
		loopCount_(0)
	{
	}
	virtual ~Tester()
	{
	}

	void Init(const char* host, int port, unsigned int interval, unsigned int session_count, unsigned int loop_count)
	{
		interval_ = interval;
		sessionCount_ = session_count;
		loopCount_ = loop_count;
		log_.Init("test", "test", 5);
		boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
		endpoint_ = *resolver_.resolve({host, Format(port).c_str()});
	}
	virtual void OnAccept(std::shared_ptr<Network::Session> session, const boost::system::error_code& error) {}
	virtual void OnRecvMsg(std::shared_ptr<Network::Session> session, std::shared_ptr<Network::Packet> packet)
	{
		std::shared_ptr<SESSION_T> test_session = std::static_pointer_cast<SESSION_T>(session);
		const unsigned int msg_id = packet->GetID();
		auto itr = mapRecvHandler_.find(msg_id);
		if(itr == mapRecvHandler_.end())
		{
			Log::Write(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
			test_session->OnError(0);
			return ;
		}

		RECV_HANDLER_TYPE& handler = itr->second;
		handler(test_session, packet);

		test_session->testSEQ_++;
		if(test_session->testSEQ_ >= (int)vecSendHandler_.size())
		{
			test_session->OnError(0);
			return;
		}
		try
		{
			vecSendHandler_[test_session->testSEQ_](test_session);
		}
		catch(const Gamnet::Exception& e)
		{
			test_session->OnError(0);
			return;
		}
		{
			std::lock_guard<std::mutex> lo(vecTestRunningState_[test_session->testSEQ_].lock_);
			vecTestRunningState_[test_session->testSEQ_].count_++;
		}
	}
	virtual void OnClose(std::shared_ptr<Network::Session> session)
	{
		sessionManager_.DelSession(session->sessionKey_, session);
	}
	void OnConnect(std::shared_ptr<SESSION_T> session)
	{
		sessionManager_.AddSession(session->sessionKey_, session);
		session->testSEQ_ = 0;
		if(session->testSEQ_ < (int)vecSendHandler_.size())
		{
			vecSendHandler_[session->testSEQ_](session);
			std::lock_guard<std::mutex> lo(vecTestRunningState_[session->testSEQ_].lock_);
			vecTestRunningState_[session->testSEQ_].count_++;
		}
	}

	template<class NTF_T>
	void RegisterHandler(RECV_HANDLER_TYPE recv)
	{
		mapRecvHandler_[NTF_T::MSG_ID] = recv;
	}
	template<class REQ_T, class ANS_T>
	void RegisterHandler(const std::string& test_name, SEND_HANDER_TYPE send, RECV_HANDLER_TYPE recv)
	{
		mapSendHandler_[test_name] = send;
		RegisterHandler<ANS_T>(recv);
	}
	void SetTestSequence(const std::string& test_name)
	{
		auto itr = mapSendHandler_.find(test_name);
		if(mapSendHandler_.end() == itr)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] can't find registered test case(test_name:", test_name, ")");
		}
		TestRunningState state;
		state.name_ = test_name;
		state.count_ = 0;
		vecTestRunningState_.push_back(state);
		vecSendHandler_.push_back(itr->second);
	}

	void Run()
	{
		if(0 == interval_)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] interval_ should be set");
		}
		if(0 == sessionCount_)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] sessionCount_ should be set");
		}
		Log::Write(GAMNET_INF, "test start...");
		timer_.SetTimer(interval_, [this]() {
			for(unsigned int i=0; i<sessionCount_; i++)
			{
				if(sessionCount_ <= (unsigned int)(this->sessionPool_.Capacity() - this->sessionPool_.Available()))
				{
					break;
				}
				threadPool_.PostTask([this]() {
					std::shared_ptr<SESSION_T> session = this->sessionPool_.Create();
					if(NULL == session)
					{
						Log::Write(GAMNET_WRN, "can not create any more session(max:", this->sessionPool_.Capacity(), ", current:", this->sessionManager_.Size(), ")");
						return;
					}

					session->listener_ = this;
					session->Connect(endpoint_);
				});
				executeCount_ += 1;
			}

			if(this->executeCount_ < loopCount_)
			{
				this->timer_.Resume();
			}
		});
		printTimer_.SetTimer(3000, [this]() {
			log_.Write(GAMNET_INF, "[Test] execute count : ", this->executeCount_);
			log_.Write(GAMNET_INF, "[Test] running session : ", this->sessionManager_.Size(), ", idle session : ", this->sessionPool_.Available());
			for(auto& itr : this->vecTestRunningState_)
			{
				log_.Write(GAMNET_INF, "[Test] running state(name : ", itr.name_, ", count : ", itr.count_, ")");
			}
			if(this->executeCount_ < loopCount_)
			{
				this->printTimer_.Resume();
			}
			else
			{
				log_.Write(GAMNET_INF, "[Test] test finished..(", this->executeCount_, "/", this->loopCount_, ")");
			}
		});
	}
};

}} /* namespace Gamnet */

#endif /* TEST_H_ */
