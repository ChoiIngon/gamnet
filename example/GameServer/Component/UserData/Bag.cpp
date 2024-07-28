#include "Bag.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include <idl/MessageUser.h>
#include <idl/MessageItem.h>
#include "../../UserSession.h"
#include "../UserData.h"
#include "Item.h"
#include "Suit.h"

namespace Component {

Bag::Bag()
	: last_item_no(0)
	, session(nullptr)
{
}

std::shared_ptr<Bag> Bag::Load(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
	if (nullptr == pUserData)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
	}

	std::shared_ptr<Component::Bag> pBag = pUserData->pBag;
	std::shared_ptr<Component::Suit> pSuit = pUserData->pSuit;

	pBag->session = session;
	pBag->last_item_no = 0;

	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT item_no, item_index, item_count, equip_part "
		"FROM UserItem "
		"WHERE user_no = ", session->user_no, " AND delete_yn='N'"
	);

	Message::Item::MsgSvrCli_AddItem_Ntf ntf;
	for (auto& row : rows)
	{
		uint32_t itemIndex = row->getUInt32("item_index");
		int32_t itemCount = row->getInt32("item_count");
		std::shared_ptr<Item::Data> item = Item::Create(itemIndex, itemCount);
		
		item->item_no = row->getInt64("item_no");
		if(nullptr != item->equip)
		{
			item->equip->part = (Message::EquipItemPartType)row->getInt32("equip_part");
			pSuit->EquipWithoutDB(item);
		}
		
		pBag->last_item_no = std::max(pBag->last_item_no, item->item_no);
		pBag->item_datas.insert(std::make_pair(item->item_no, item));

		Message::ItemData msgItemData;
		msgItemData.item_index = item->meta->Index;
		msgItemData.item_no = item->item_no;
		msgItemData.item_count = item->count;
		ntf.item_datas.push_back(msgItemData);
	}

	if(0 < ntf.item_datas.size())
	{
		Gamnet::Network::Tcp::SendMsg(session, ntf, true);
	}

	return pBag;
}

std::shared_ptr<Transaction::Statement> Bag::Insert(const std::shared_ptr<Item::Data>& item)
{
	std::shared_ptr<InsertStatement> insert = std::make_shared<InsertStatement>();
	insert->session = this->session;

	const std::shared_ptr<Item::Meta>& meta = item->meta;

	if (1 < meta->MaxStack)	// stackable item
	{
		int stackCount = item->count / meta->MaxStack;	// 신규 아이템을 최대 스택 카운트 별로 쪼개 생성
		for (int i = 0; i < stackCount; i++)
		{
			std::shared_ptr<Item::Data> portion = meta->CreateInstance();
			portion->item_no = ++last_item_no;
			portion->count = meta->MaxStack;
			insert->insert_itmes.push_back(portion);
		}

		item->count -= item->meta->MaxStack * stackCount;
	}

	if(0 < item->count)
	{
		item->item_no = ++last_item_no;
		insert->insert_itmes.push_back(item);
	}

	for (std::shared_ptr<Item::Data> data : insert->insert_itmes)
	{
		this->item_datas.insert(std::make_pair(data->item_no, data));
	}

	return insert;
}

void Bag::InsertStatement::Commit(const std::shared_ptr<Transaction::Connection>& db)
{
	std::string query = "INSERT INTO UserItem(`user_no`, `item_no`, `item_index`, `item_count`, `equip_part`, `delete_yn`) VALUES ";
	for (std::shared_ptr<Item::Data> item : insert_itmes)
	{
		query += Gamnet::Format("(", session->user_no, ",", item->item_no, ",", item->meta->Index, ",", item->count, ",0,'N')");
	}

	db->Execute(query);
}

void Bag::InsertStatement::Rollback()
{
	std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
	std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

	for (std::shared_ptr<Item::Data> item : insert_itmes)
	{
		pBag->Remove(item->item_no, item->count);
	}
}

void Bag::InsertStatement::Sync()
{
	Message::Item::MsgSvrCli_AddItem_Ntf ntf;
	for (std::shared_ptr<Item::Data> item : insert_itmes)
	{
		Message::ItemData msgItemData;
		msgItemData.item_index = item->meta->Index;
		msgItemData.item_no = item->item_no;
		msgItemData.item_count = item->count;
		ntf.item_datas.push_back(msgItemData);
	}

	if (0 < ntf.item_datas.size())
	{
		Gamnet::Network::Tcp::SendMsg(session, ntf, true);
	}
}

std::shared_ptr<Item::Data> Bag::Find(int64_t itemNo)
{
	auto itr = item_datas.find(itemNo);
	if(item_datas.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<Transaction::Statement> Bag::Remove(int64_t itemNo, int count)
{
	std::shared_ptr<DeleteStatement> remove = std::make_shared<DeleteStatement>();
	remove->session = this->session;
	remove->data = Find(itemNo);
	remove->count = count;

	if (nullptr == remove->data)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	if (count > remove->data->count)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	if (true == remove->data->count.Stackable())
	{
		remove->data->count -= count;
	}

	if (0 == remove->data->count)
	{
		this->item_datas.erase(itemNo);
	}

	return remove;
}

void Bag::DeleteStatement::Commit(const std::shared_ptr<Transaction::Connection>& db)
{
	db->Execute(
		Gamnet::Format("UPDATE UserItem SET `item_count` = ", data->count, ",`delete_yn`=", (0 == data->count ? "'Y'" : "'N'"), " WHERE user_no=", session->user_no, " AND item_no=", data->item_no)
	);
}

void Bag::DeleteStatement::Rollback()
{
	std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
	std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

	data->count += count;

	auto itr = pBag->item_datas.find(data->item_no);
	if (pBag->item_datas.end() == itr)
	{
		pBag->item_datas.insert(std::make_pair(data->item_no, data));
	}
}

void Bag::DeleteStatement::Sync()
{
	Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
	Message::ItemData msgItemData;
	msgItemData.item_index = data->meta->Index;
	msgItemData.item_no = data->item_no;
	msgItemData.item_count = data->count;
	ntf.item_datas.push_back(msgItemData);

	Gamnet::Network::Tcp::SendMsg(session, ntf, true);
}

}

void Test_AddItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_AddItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& item : ntf.item_datas)
	{
		session->items.insert(std::make_pair((int)item.item_no, item));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_AddItem_Ntf, Test_AddItem_Ntf
);

void Test_UpdateItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& item : ntf.item_datas)
	{
		if (0 == item.item_count)
		{
			session->items.erase(item.item_no);
		}
		else
		{
			session->items[item.item_no] = item;
		}
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_UpdateItem_Ntf, Test_UpdateItem_Ntf
);
