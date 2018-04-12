#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../../Library/Timer.h"

namespace Gamnet { namespace Network { namespace Router {

class Session : public Network::Tcp::Session {
private :
	struct AnswerWatingSessionManager
	{
	private :
		std::mutex lock_;
		std::map<uint64_t, std::pair<time_t, std::shared_ptr<Network::Tcp::Session>>> mapSession_;
		Timer timer_;
	public :
		AnswerWatingSessionManager()
		{
			timer_.SetTimer(10000, [this](){
				std::lock_guard<std::mutex> lo(this->lock_);
				time_t now_ = time(NULL);
				for(auto itr = this->mapSession_.begin(); itr != this->mapSession_.end();) {
					std::pair<time_t, std::shared_ptr<Network::Session>> pair_ = itr->second;
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

		bool AddSession(uint64_t msg_seq, std::shared_ptr<Network::Tcp::Session> session)
		{
			std::lock_guard<std::mutex> lo(lock_);
			return mapSession_.insert(std::make_pair(msg_seq, std::make_pair(time(NULL), session))).second;
		}

		std::shared_ptr<Network::Tcp::Session> FindSession(uint64_t msg_seq)
		{
			std::lock_guard<std::mutex> lo(lock_);
			auto itr = mapSession_.find(msg_seq);
			if(mapSession_.end() == itr)
			{
				return NULL;
			}
			std::shared_ptr<Network::Tcp::Session> session = itr->second.second;
			mapSession_.erase(itr);
			return session;
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lo(lock_);
			mapSession_.clear();
		}
	};
public:
	/*
	struct Init {
		Session* operator() (Session* session);
	};
	*/
	AnswerWatingSessionManager watingSessionManager_;

	Session();
	virtual ~Session();

	Address address;
	std::function<void(const Address& addr)> onRouterConnect;
	std::function<void(const Address& addr)> onRouterClose;

	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
