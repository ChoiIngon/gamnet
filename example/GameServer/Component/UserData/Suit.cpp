#include "Suit.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include <idl/MessageUser.h>
#include <idl/MessageItem.h>
#include "../../UserSession.h"
#include "../UserData.h"
#include "Item.h"

namespace Component {

Suit::Suit()
{
	for (int i = 0; i < (int)Message::EquipItemPartType::Max; i++)
	{
		item_datas[i] = nullptr;
	}
}

void Suit::Equip(const std::shared_ptr<Item::Data>& item)
{
    if (nullptr != item_datas[(int)item->equip->part])
    {
        Unequip(item->equip->part);
    }

	item_datas[(int)item->equip->part] = item;
}

void Suit::Serialize(std::list<Message::EquipItemData>& items) const
{
	for (int i = 0; i < (int)Message::EquipItemPartType::Max; i++)
	{
		auto pItem = item_datas[i];
		if (nullptr != pItem)
		{
			Message::EquipItemData item;
			item.item_no = pItem->item_no;
			item.item_index = pItem->meta->Index;
			item.item_count = pItem->count;
			item.part_type = (Message::EquipItemPartType)i;

			items.push_back(item);
		}
	}
}

	void Suit::Unequip(Message::EquipItemPartType part)
	{
		/*
		std::shared_ptr<UserSession> session = this->session;
		std::shared_ptr<Item::Data> item = Find(part);
		if (nullptr == item)
		{
			return;
		}
		
		session->queries->Update("user_item",
			Gamnet::Format("equip_part=", (int)Message::EquipItemPartType::Invalid),
			{
				{ "user_seq", session->user_no },
				{ "item_seq", item->item_no }
			}
		);
		
		session->on_commit.push_back([=]()
		{
			item_datas[(int)part] = nullptr;
			item->equip->part = Message::EquipItemPartType::Invalid;
			Message::Item::MsgSvrCli_UnequipItem_Ntf ntf;
			ntf.part_type = part;
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		});
		*/
	}

	std::shared_ptr<Item::Data> Suit::Find(Message::EquipItemPartType part)
	{
		return item_datas[(int)part];
	}
}
/*

void Test_UnequipItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_UnequipItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_UnequipItem_Ntf, Test_UnequipItem_Ntf
);

*/