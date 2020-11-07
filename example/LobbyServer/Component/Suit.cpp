#include "Suit.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include <idl/MessageUser.h>
#include <idl/MessageItem.h>
#include "../UserSession.h"
#include "Item.h"

namespace Component {
	Suit::Suit(const std::shared_ptr<UserSession>& session)
		: session(session)
	{
	}

	void Suit::Equip(const std::shared_ptr<Item::Data>& item)
	{
		const std::shared_ptr<Item::Meta>& meta = item->meta;
		assert(nullptr != meta->equip);

		if(nullptr == item->equip)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		Unequip(meta->equip->part);

		if (nullptr != item->expire)
		{
			item->expire->TriggerExpire(Item::Meta::Expire::TriggerType::OnEquip);
		}

		session->queries->Update("user_item",
			Gamnet::Format("equip_part=", (int)meta->equip->part, ",expire_date='", item->GetExpireDate().ToString(), "'"),
			{
				{ "user_seq", session->user_seq },
				{ "item_seq", item->seq }
			}
		);

		session->on_commit.push_back([=](){
			item->equip->part = meta->equip->part;
			item_datas[(int)meta->equip->part] = item;

			Message::Item::MsgSvrCli_EquipItem_Ntf ntf;
			ntf.item_seq = item->seq;
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		});
	}

	void Suit::Unequip(Message::EquipItemPartType part)
	{
		std::shared_ptr<UserSession> session = this->session;
		std::shared_ptr<Item::Data> item = Find(part);
		if (nullptr == item)
		{
			return;
		}
		
		session->queries->Update("user_item",
			Gamnet::Format("equip_part=", (int)Message::EquipItemPartType::Invalid),
			{
				{ "user_seq", session->user_seq },
				{ "item_seq", item->seq }
			}
		);
		
		session->on_commit.push_back([=]()
		{
			item_datas[(int)part] = nullptr;
			item->equip->part = Message::EquipItemPartType::Invalid;

			Message::Item::MsgSvrCli_UnequipItem_Ntf ntf;
			ntf.item_seq = item->seq;
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		});
	}

	std::shared_ptr<Item::Data> Suit::Find(Message::EquipItemPartType part)
	{
		return item_datas[(int)part];
	}
}

void Test_EquipItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_EquipItem_Ntf ntf;
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
	Message::Item::MsgSvrCli_EquipItem_Ntf, Test_EquipItem_Ntf
);

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

