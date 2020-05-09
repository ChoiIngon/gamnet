#include "Manager_CastGroup.h"

Manager_CastGroup::Manager_CastGroup() {
}

Manager_CastGroup::~Manager_CastGroup() {
}

void Manager_CastGroup::Init()
{
}

GAMNET_BIND_INIT_HANDLER(Manager_CastGroup, Init);

std::shared_ptr<Gamnet::Network::Tcp::CastGroup> Manager_CastGroup::JoinCastGroup(std::shared_ptr<UserSession> session)
{
	std::lock_guard<std::mutex> lo(lock);
	if(nullptr == cast_group)
	{
		cast_group = Gamnet::Network::Tcp::CastGroup::Create();
	}

	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> ret = cast_group;

	Gamnet::Network::Tcp::CastGroup::LockGuard lockedCastGroup(cast_group);
	if (CHAT_QUORUM <= cast_group->Insert(session))
	{
		cast_group = nullptr;
	}		
	return ret;
}

