#ifndef MANAGER_CHANNEL_H_
#define MANAGER_CHANNEL_H_

#include "UserSession.h"

class Manager_CastGroup 
{
private:
	std::mutex lock;

	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> cast_group;
public:
	Manager_CastGroup();
	~Manager_CastGroup();

	void Init();
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> JoinCastGroup(std::shared_ptr<UserSession> session);
};

#endif /* MANAGER_SESSION_H_ */
