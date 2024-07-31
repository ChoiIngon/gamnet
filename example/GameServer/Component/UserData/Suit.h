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
		friend void Item::Load(const std::shared_ptr<UserSession>&);

	public:
		Suit();

		void Equip(const std::shared_ptr<Item::Data>& item);
		void Unequip(Message::EquipItemPartType part);
		std::shared_ptr<Item::Data> Find(Message::EquipItemPartType part);

		void Serialize(std::list<Message::EquipItemData>& items) const;
	private:
		std::shared_ptr<UserSession> session;
		std::shared_ptr<Item::Data> item_datas[(int)Message::EquipItemPartType::Max];

	};

};
#endif
