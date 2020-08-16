#ifndef _COMPONENT_BAG_H_
#define _COMPONENT_BAG_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <map>

class UserSession;

namespace Component {
	struct ItemData;
	class Bag
	{
	public :
		Bag(const std::shared_ptr<UserSession>& session);
		void Load();
		void Insert(const std::shared_ptr<ItemData>& item);
		std::shared_ptr<ItemData> Find(uint64_t itemSEQ);
		void Remove(uint64_t itemSEQ, int count);
	private :
		std::shared_ptr<UserSession> session;
		std::map<uint64_t, std::shared_ptr<ItemData>> item_datas;
		uint64_t last_item_seq;
	};
};
#endif
