#ifndef _COMPONENT_BAG_H_
#define _COMPONENT_BAG_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <map>

class UserSession;

namespace Item {
	struct Meta;
	class Data;
}
namespace Component {
	
	class Bag
	{
	public :
		Bag(const std::shared_ptr<UserSession>& session);

		void Load();

		void Insert(const std::shared_ptr<Item::Data>& item);
		std::shared_ptr<Item::Data> Find(uint64_t itemSEQ);
		void Remove(uint64_t itemSEQ, int count);

	private :
		
		std::shared_ptr<UserSession> session;
		uint64_t last_item_seq;
		std::map<uint64_t, std::shared_ptr<Item::Data>> item_datas;
	};
};
#endif
