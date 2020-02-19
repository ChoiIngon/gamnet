#include "Manager_CastGroup.h"

Manager_CastGroup::Manager_CastGroup() {
}

Manager_CastGroup::~Manager_CastGroup() {
}

void Manager_CastGroup::Init()
{
}

GAMNET_BIND_INIT_HANDLER(Manager_CastGroup, Init);

std::shared_ptr<Gamnet::Network::Tcp::CastGroup> Manager_CastGroup::GetCastGroup()
{
	std::lock_guard<std::mutex> lo(lock);
	if(0 == cast_groups.size())
	{
		cast_groups.push_back(Gamnet::Network::Tcp::CastGroup::Create());
	}
	
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> group = cast_groups.front();
	cast_groups.pop_front();
	return group;
}

void Manager_CastGroup::AddCastGroup(std::shared_ptr<Gamnet::Network::Tcp::CastGroup> group)
{
	std::lock_guard<std::mutex> lo(lock);
	cast_groups.push_front(group);
}
