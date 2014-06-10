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
#include "MsgRouter.h"

namespace Gamnet { namespace Router {

class Session : public Network::Session {
private :
	struct AnswerWatingSessionManager
	{
	private :
		std::mutex lock_;
		std::map<uint64_t, std::shared_ptr<Network::Session>> mapSession_;
	public :
		AnswerWatingSessionManager(){}
		~AnswerWatingSessionManager() {}

		bool AddSession(uint64_t msg_seq, std::shared_ptr<Network::Session> session)
		{
			std::lock_guard<std::mutex> lo(lock_);
			return mapSession_.insert(std::make_pair(msg_seq, session)).second;
		}

		std::shared_ptr<Network::Session> FindSession(uint64_t msg_seq)
		{
			std::lock_guard<std::mutex> lo(lock_);
			auto itr = mapSession_.find(msg_seq);
			if(mapSession_.end() == itr)
			{
				return NULL;
			}
			std::shared_ptr<Network::Session> session = itr->second;
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
	virtual void OnConnect();


};

}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
