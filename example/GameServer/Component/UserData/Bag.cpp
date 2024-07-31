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

Bag::InsertResult Bag::Insert(const std::shared_ptr<Item::Data>& item)
{
	std::list<std::shared_ptr<Item::Data>> insert_items;

	if(0 != item->item_no)
    {
		this->item_datas.insert(std::make_pair(item->item_no, item));
        insert_items.push_back(item);
		return insert_items;
    }

	const std::shared_ptr<Item::Meta>& meta = item->meta;

	if (1 < meta->MaxStack)	// stackable item
	{
		int stackCount = item->count / meta->MaxStack;	// 신규 아이템을 최대 스택 카운트 별로 쪼개 생성
		for (int i = 0; i < stackCount; i++)
		{
			std::shared_ptr<Item::Data> portion = meta->CreateInstance();
			portion->item_no = ++last_item_no;
			portion->count = meta->MaxStack;
			insert_items.push_back(portion);
		}

		item->count -= item->meta->MaxStack * stackCount;
	}

	if(0 < item->count)
	{
		item->item_no = ++last_item_no;
		insert_items.push_back(item);
	}

	for (std::shared_ptr<Item::Data> data : insert_items)
	{
		this->item_datas.insert(std::make_pair(data->item_no, data));
	}

	return insert_items;
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

Bag::RemoveResult Bag::Remove(int64_t itemNo, int count)
{
	auto pItem = Find(itemNo);
	if (nullptr == pItem)
    {
		return (int)Message::ErrorCode::UndefineError;
    }

	if (count > pItem->count)
	{
		return (int)Message::ErrorCode::UndefineError;
	}

	if (true == pItem->count.Stackable())
	{
		pItem->count -= count;
	}
	else
	{
		this->item_datas.erase(itemNo);
		return pItem;
	}

	if (0 == pItem->count)
	{
		this->item_datas.erase(itemNo);
	}

	return pItem;
}

void Bag::Serialize(std::list<Message::ItemData>& items) const
{
	for (auto itr : item_datas)
	{
		auto pItem = itr.second;
		
		Message::ItemData item;
		item.item_no = pItem->item_no;
		item.item_index = pItem->meta->Index;
		item.item_count = pItem->count;

		items.push_back(item);
	}
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

		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		auto pBag = pUserData->pBag;

		for (auto& item : ntf.item_datas)
		{
			std::shared_ptr<Item::Data> data = Item::Create(item.item_index, item.item_count);
			data->item_no = item.item_no;
			pBag->Insert(data);
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
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

		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		auto pBag = pUserData->pBag;

		for (auto& item : ntf.item_datas)
		{
			auto pItemData = pBag->Find(item.item_no);
			if (0 == item.item_count)
			{
				pBag->Remove(pItemData->item_no, pItemData->count);
			}
			else
			{
				auto pItemData = pBag->Find(item.item_no);
				pItemData->count = item.item_count;
			}
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_UpdateItem_Ntf, Test_UpdateItem_Ntf
);
