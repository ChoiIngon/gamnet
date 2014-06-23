/*
 * ServerSession.h
 *
 *  Created on: Jun 6, 2014
 *      Author: kukuta
 */

#ifndef SERVERSESSION_H_
#define SERVERSESSION_H_

#include "../Network/Network.h"
#include "../Library/Timer.h"


namespace Gamnet { namespace Router {
#include "MsgRouter.h"
class Session : public Network::Session {
private :
	struct AnswerWatingSessionManager
	{
	private :
		std::mutex lock_;
		std::map<uint64_t, std::pair<time_t, std::shared_ptr<Network::Session>>> mapSession_;
		Timer timer_;
	public :
		AnswerWatingSessionManager()
		{
			timer_.SetTimer(10000, [this](){
				std::lock_guard<std::mutex> lo(this->lock_);
				time_t now_ = time(NULL);
				for(auto itr = this->mapSession_.begin(); itr != this->mapSession_.end();) {
					std::pair<uint64_t, std::shared_ptr<Network::Session>> pair_ = itr->second;
					if(pair_.first + 5 < now_)
					{
						Log::Write(GAMNET_ERR, "[Router] idle session timeout");
						this->mapSession_.erase(itr++);
				    }
				    else {
				        ++itr;
				    }
				}
				this->timer_.Resume();
			});
		}
		~AnswerWatingSessionManager() {}

		bool AddSession(uint64_t msg_seq, std::shared_ptr<Network::Session> session)
		{
			std::lock_guard<std::mutex> lo(lock_);
			return mapSession_.insert(std::make_pair(msg_seq, std::make_pair(time(NULL), session))).second;
		}

		std::shared_ptr<Network::Session> FindSession(uint64_t msg_seq)
		{
			std::lock_guard<std::mutex> lo(lock_);
			auto itr = mapSession_.find(msg_seq);
			if(mapSession_.end() == itr)
			{
				return NULL;
			}
			std::shared_ptr<Network::Session> session = itr->second.second;
			mapSession_.erase(itr);
			return session;
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lo(lock_);
			mapSession_.clear();
		}
	};

	Timer timer_;
public:
	AnswerWatingSessionManager watingSessionManager_;

	Session();
	virtual ~Session();

	Address addr;

	void Init();
	void Connect(const char* host, int port, int timeout = 5);
	virtual void OnAccept() {}
	virtual void OnConnect();
	virtual void OnClose(int reason);
};

}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
