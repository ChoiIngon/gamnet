#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../../Library/Timer.h"

namespace Gamnet { namespace Network { namespace Router {

class Session : public Network::Tcp::Session 
{
private :
	struct AnswerWatingSessionManager
	{
	private :
		std::mutex lock_;
		std::map<uint64_t, std::pair<time_t, std::shared_ptr<Network::Tcp::Session>>> wait_sessions_;
		std::shared_ptr<Time::Timer> timer_;
	public :
		AnswerWatingSessionManager();
		~AnswerWatingSessionManager();

		void Init();
		bool AddSession(uint64_t recv_seq, std::shared_ptr<Network::Tcp::Session> session);
		std::shared_ptr<Network::Tcp::Session> FindSession(uint64_t recv_seq);
		void Clear();
	};
public:

	Session();
	virtual ~Session();

	Address address;
	AnswerWatingSessionManager watingSessionManager_;
	
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
