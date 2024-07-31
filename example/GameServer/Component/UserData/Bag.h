#ifndef _COMPONENT_BAG_H_
#define _COMPONENT_BAG_H_

#include <memory>
#include <map>
#include "../../Util/Transaction.h"
#include "../../Component/UserData/Item.h"
#include <Gamnet/Library/Return.h>

class UserSession;

namespace Item {
	struct Meta;
	class Data;
	void Load(const std::shared_ptr<UserSession>&);
}

namespace Component {
	
class Bag
{
	friend void Item::Load(const std::shared_ptr<UserSession>&);
public :
	Bag();

	typedef	std::list<std::shared_ptr<Item::Data>> ItemList;
	typedef Gamnet::Return<ItemList> InsertResult;
	typedef Gamnet::Return<std::shared_ptr<Item::Data>> RemoveResult;

	InsertResult Insert(const std::shared_ptr<Item::Data>& item);
	RemoveResult Remove(int64_t itemNo, int count);
	std::shared_ptr<Item::Data>	Find(int64_t itemNo);

	void Serialize(std::list<Message::ItemData>& items) const;
private :
	int64_t last_item_no;
	std::shared_ptr<UserSession> session;
	std::map<int64_t, std::shared_ptr<Item::Data>> item_datas;
};

}
#endif