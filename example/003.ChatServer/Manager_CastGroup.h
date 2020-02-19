#ifndef MANAGER_CHANNEL_H_
#define MANAGER_CHANNEL_H_

#include <Gamnet.h>

class Manager_CastGroup 
{
private:
	std::mutex lock;

	std::deque<std::shared_ptr<Gamnet::Network::Tcp::CastGroup>> cast_groups;
public:
	Manager_CastGroup();
	~Manager_CastGroup();

	void Init();
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> GetCastGroup();
	void AddCastGroup(std::shared_ptr<Gamnet::Network::Tcp::CastGroup> group);
};

#endif /* MANAGER_SESSION_H_ */
