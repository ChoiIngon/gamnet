#ifndef _COMPONENT_BAG_H_
#define _COMPONENT_BAG_H_

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
	Bag();
/*
	void Insert(const std::shared_ptr<Item::Data>& item);
	std::shared_ptr<Item::Data> Find(uint64_t itemSEQ);
	void Remove(uint64_t itemSEQ, int count);
*/
	static std::shared_ptr<Bag> Load(const std::shared_ptr<UserSession>& session);
private :
	int64_t last_item_no;
	std::map<uint64_t, std::shared_ptr<Item::Data>> item_datas;
};
};
#endif
