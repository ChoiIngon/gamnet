#include "LinkManager.h"
#include "../Library/Singleton.h"
#include <list>

namespace Gamnet { namespace Network {

LinkManager::LinkManager() : name("Gamnet::Network::LinkManager"), expire_time(0)
{
}

LinkManager::~LinkManager()
{
}

bool LinkManager::Add(const std::shared_ptr<Link>& link)
{
	std::lock_guard<std::mutex> lo(lock);
	return links.insert(std::make_pair(link->link_key, link)).second;
}

void LinkManager::Remove(uint32_t linkKey)
{
	std::lock_guard<std::mutex> lo(lock);
	links.erase(linkKey);
}

size_t LinkManager::Size()
{
	std::lock_guard<std::mutex> lo(lock);
	return links.size();
}

void LinkManager::ActivateIdleLinkTerminator(int timeout)
{
	expire_time = timeout;
	expire_timer.Cancel();
	if(0 < expire_time)
	{
		expire_timer.AutoReset(true);
		expire_timer.SetTimer(expire_time * 1000, std::bind(&LinkManager::OnTimerExpire, this));
	}
}

void LinkManager::OnTimerExpire()
{
	std::list<std::shared_ptr<Link>> linksToBeDeleted;
	{
		std::lock_guard<std::mutex> lo(lock);
		int64_t now = time(nullptr);
		
		for (auto itr = links.begin(); itr != links.end(); itr++)
		{
			std::shared_ptr<Link> link = itr->second;
			if (link->expire_time + expire_time < now)
			{
				linksToBeDeleted.push_back(link);
			}
		}
	}

	for (auto link : linksToBeDeleted)
	{
		LOG(GAMNET_ERR, "[", name, "/", link->link_key, "/0] destroy idle link");
		link->strand.wrap(std::bind(&Link::Close, link, ErrorCode::IdleTimeoutError))();
	}
}
}} 
