#ifndef _COMPONENT_SUIT_H_
#define _COMPONENT_SUIT_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <idl/MessageCommon.h>

class UserSession;

namespace Item {
	class Data;
};

namespace Component {

	class Suit
	{
	public:
		Suit(const std::shared_ptr<UserSession>& session);

		void EquipWithoutDB(const std::shared_ptr<Item::Data>& item);
		void Equip(const std::shared_ptr<Item::Data>& item);
		void Unequip(Message::EquipItemPartType part);
		std::shared_ptr<Item::Data> Find(Message::EquipItemPartType part);
	private:
		std::shared_ptr<UserSession> session;
		std::shared_ptr<Item::Data> item_datas[(int)Message::EquipItemPartType::Max];
	};
};
#endif
