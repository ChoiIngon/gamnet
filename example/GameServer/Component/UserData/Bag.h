#ifndef _COMPONENT_BAG_H_
#define _COMPONENT_BAG_H_

#include <memory>
#include <map>
#include "../../Util/Transaction.h"
#include "../../Component/UserData/Item.h"

class UserSession;

namespace Item {
	struct Meta;
	class Data;
	void Load(const std::shared_ptr<UserSession>&);
}

namespace Component {
	
class Bag
{
	class InsertStatement : public Transaction::Statement
	{
	public:
		std::shared_ptr<UserSession> session;
		std::list<std::shared_ptr<Item::Data>> insert_itmes;

		virtual void Commit(const std::shared_ptr<Transaction::Connection>& db) override;
		virtual void Rollback() override;
		virtual void Sync() override;
	};

	class DeleteStatement : public Transaction::Statement
	{
	public :
		std::shared_ptr<UserSession> session;
		std::shared_ptr<Item::Data> data;
		int count;

		virtual void Commit(const std::shared_ptr<Transaction::Connection>& db) override;
		virtual void Rollback() override;
		virtual void Sync() override;
	};

	friend void Item::Load(const std::shared_ptr<UserSession>&);
public :
	Bag();

	std::shared_ptr<Transaction::Statement> Insert(const std::shared_ptr<Item::Data>& item);
	std::shared_ptr<Item::Data> Find(int64_t itemNo);
	std::shared_ptr<Transaction::Statement> Remove(int64_t itemNo, int count);

	void Serialize(std::list<Message::ItemData>& items) const;
private :
	int64_t last_item_no;
	std::shared_ptr<UserSession> session;
	std::map<int64_t, std::shared_ptr<Item::Data>> item_datas;
};

}
#endif