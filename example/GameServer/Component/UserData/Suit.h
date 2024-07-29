#ifndef _COMPONENT_SUIT_H_
#define _COMPONENT_SUIT_H_

#include <memory>
#include <idl/MessageCommon.h>
#include "../../Util/Transaction.h"
#include "../../UserSession.h"

namespace Item {
	class Data;
	void Load(const std::shared_ptr<UserSession>& session);
};

namespace Component {

	class Suit
	{
		class EquipStatement : public Transaction::Statement
		{
		public :
			std::shared_ptr<UserSession> session;
			std::shared_ptr<Item::Data> item;
			virtual void Commit(const std::shared_ptr<Transaction::Connection>& db) override;
			virtual void Rollback() override;
			virtual void Sync() override;
		};

		friend void Item::Load(const std::shared_ptr<UserSession>&);

	public:
		Suit();
		
		std::shared_ptr<Transaction::Statement> Equip(const std::shared_ptr<Item::Data>& item);
		void EquipWithoutDB(const std::shared_ptr<Item::Data>& item);
		void Unequip(Message::EquipItemPartType part);
		std::shared_ptr<Item::Data> Find(Message::EquipItemPartType part);

		void Serialize(std::list<Message::EquipItemData>& items) const;
	private:
		std::shared_ptr<UserSession> session;
		std::shared_ptr<Item::Data> item_datas[(int)Message::EquipItemPartType::Max];

	};

};
#endif
