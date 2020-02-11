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
	
	for(auto itr : cast_groups)
	{
		cast_groups.erase(itr.first);
		return itr.second;
	}

	return Gamnet::Network::Tcp::CastGroup::Create();
}

void Manager_CastGroup::AddCastGroup(std::shared_ptr<Gamnet::Network::Tcp::CastGroup> group)
{
	std::lock_guard<std::mutex> lo(lock);
	cast_groups.insert(std::make_pair(group->group_seq, group));
}
