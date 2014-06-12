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

	std::vector<SEND_HANDER_TYPE> vecSendHandler_;
	std::map<unsigned int, RECV_HANDLER_TYPE> mapRecvHandler_;

	Pool<SESSION_T, std::mutex, Network::Session::Init> sessionPool_;
	Timer timer_;
	ThreadPool threadPool_;
	unsigned int executeCount_;

public :
	unsigned int elapsedTime_;
	unsigned int sessionCount_;
	unsigned int loopCount_;
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::endpoint endpoint_;
public :
	Tester() :
		sessionPool_(1024),
		threadPool_(30),
		executeCount_(0),
		elapsedTime_(0),
		sessionCount_(0),
		loopCount_(0),
		resolver_(Singleton<boost::asio::io_service>())
	{
	}
	virtual ~Tester()
	{
	}

	virtual void OnRecvMsg(std::shared_ptr<Network::Session> session, std::shared_ptr<Network::Packet> packet)
	{
		std::shared_ptr<SESSION_T> test_session = std::static_pointer_cast<SESSION_T>(session);
		const unsigned int msg_id = packet->GetID();
		auto itr = mapRecvHandler_.find(msg_id);
		if(itr == mapRecvHandler_.end())
		{
			Log::Write(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
			return ;
		}

		RECV_HANDLER_TYPE& handler = itr->second;
		handler(test_session, packet);

		test_session->testSEQ_++;
		if(test_session->testSEQ_ >= vecSendHandler_.size())
		{
			test_session->OnError(0);
			return;
		}
		vecSendHandler_[test_session->testSEQ_](test_session);
	}
	virtual void OnClose(std::shared_ptr<Network::Session> session)
	{
		sessionManager_.DelSession(session->sessionKey_, session);
	}
	void OnConnect(std::shared_ptr<SESSION_T> session)
	{
		sessionManager_.AddSession(session->sessionKey_, session);
		session->testSEQ_ = 0;
		if(session->testSEQ_ < vecSendHandler_.size())
		{
			vecSendHandler_[session->testSEQ_](session);
		}
	}

	template<class NTF_T>
	void RegisterHandler(RECV_HANDLER_TYPE recv)
	{
		mapRecvHandler_[NTF_T::MSG_ID] = recv;
	}
	template<class REQ_T, class ANS_T>
	void RegisterHandler(SEND_HANDER_TYPE send, RECV_HANDLER_TYPE recv)
	{
		vecSendHandler_.push_back(send);
		RegisterHandler<ANS_T>(recv);
	}

	void Run(const char* host, int port)
	{
		endpoint_ = *resolver_.resolve({host, String(port).c_str()});
		if(0 == elapsedTime_)
		{
			throw GAMNET_EXCEPTION("elapsedTime_ should be set", 0);
		}
		if(0 == sessionCount_)
		{
			throw GAMNET_EXCEPTION("sessionCount_ should be set", 0);
		}
		Log::Write(GAMNET_INF, "test start...");
		timer_.SetTimer(elapsedTime_, [this, host, port]() {
			for(unsigned int i=0; i<sessionCount_; i++)
			{
				if(sessionCount_ <= (unsigned int)(this->sessionPool_.Capacity() - this->sessionPool_.Available()))
				{
					break;
				}
				threadPool_.PostTask([this, host, port]() {
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

			if(executeCount_ % 10000)
			{
				Log::Write(GAMNET_INF, "Test execute count(", executeCount_, ")");
			}
			if(executeCount_ < loopCount_)
			{
				this->timer_.Resume();
			}
			else
			{
				Log::Write(GAMNET_INF, "test finished...");
			}
		});
	}
};

}} /* namespace Gamnet */

#endif /* TEST_H_ */
