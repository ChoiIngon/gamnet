#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <Gamnet/Network/Tcp/Tcp.h>
#include <IDL/MessageItem.h>
#include "../../UserSession.h"
#include "Bag.h"
#include "../UserCounter.h"
#include "../UserData.h"
#include "../UserData/Suit.h"
#include "../UserData/Bag.h"

namespace Item {

	Meta::Equip::Equip()
		: Part(Message::EquipItemPartType::Invalid)
		, Attack(0)
		, Defense(0)
		, Speed(0)
	{
		META_CUSTOM(Part, Equip::OnPartType);
		META_MEMBER(Attack);
		META_MEMBER(Defense);
		META_MEMBER(Speed);
	}
				 
	void Meta::Equip::OnPartType(Message::EquipItemPartType& part, const std::string& value)
	{
		part = Message::Parse<Message::EquipItemPartType>(value);
	}

	Meta::Price::Price()
		: Type(Message::CounterType::Invalid)
		, Value(0)
	{
		META_CUSTOM(Type, Price::OnPriceType);
		META_MEMBER(Value);
	}

	void Meta::Price::OnPriceType(Message::CounterType& type, const std::string& value)
	{
		type = Message::Parse<Message::CounterType>(value);
	}

	Meta::Expire::Expire()
		: TriggerType(ETriggerType::None)
		, ExpireType(EExpireType::Infinite)
		, Time(0)
		, Date(0)
	{
		META_CUSTOM(TriggerType, Expire::OnTriggerType);
		META_CUSTOM(ExpireType, Expire::OnExpireType);
		META_MEMBER(Time);
		META_MEMBER(Date);
	}

	void Meta::Expire::OnTriggerType(ETriggerType& triggerType, const std::string& value)
	{
		triggerType = ETriggerType::None;

		if ("OnCreate" == value)
		{
			triggerType = ETriggerType::OnCreate;
			return;
		}
		else if ("OnEquip" == value)
		{
			triggerType = ETriggerType::OnEquip;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	void Meta::Expire::OnExpireType(EExpireType& expireType, const std::string& value)
	{
		expireType = EExpireType::Infinite;

		if ("Infinite" == value)
		{
			expireType = EExpireType::Infinite;
			return;
		}
		else if ("DueDate" == value)
		{
			expireType = EExpireType::DueDate;
			return;
		}
		else if ("Period" == value)
		{
			expireType = EExpireType::Period;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	Meta::Package::Package()
		: Code("")
		, Count(0)
	{
		META_MEMBER(Code);
		META_MEMBER(Count);
	}
	
	Meta::Meta()
		: Code("")
		, Index(0)
		, Type(Message::ItemType::Invalid)
		, Grade(0)
		, MaxStack(0)
	{
		META_MEMBER(Code);
		META_MEMBER(Index);
		META_CUSTOM(Type, Meta::OnItemType);
		META_MEMBER(Grade);
		META_MEMBER(MaxStack);
		META_MEMBER(Equip);
		META_MEMBER(Price);
		META_MEMBER(Expire);
		META_MEMBER(Package);
	}

	void Meta::OnLoad() 
	{
		if(Message::ItemType::Equip == Type && nullptr == Equip)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(Message::ItemType::Package == Type && 0 == Package.size())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(nullptr != Expire)
		{
			if(Expire::ETriggerType::None == Expire->TriggerType || Expire::EExpireType::Infinite == Expire->ExpireType)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
		}
	}
	void Meta::OnItemType(Message::ItemType& itemType, const std::string& value)
	{
		itemType = Message::Parse<Message::ItemType>(value);
	}

	std::shared_ptr<Data> Meta::CreateInstance()
	{
		std::shared_ptr<Data> item = std::make_shared<Data>(shared_from_this());
		if(nullptr != Equip)
		{
			item->equip = std::make_shared<Data::Equip>();
		}

		if (0 < Package.size())
		{
			item->package = std::make_shared<Data::Package>(item);
		}

		return item;
	}
	
	Data::Equip::Equip()
		: part(Message::EquipItemPartType::Invalid)
	{
	}

	Data::Data(const std::shared_ptr<Meta>& meta)
		: meta(meta)
		, item_no(0)
		, equip(nullptr)
		, package(nullptr)
		, count(meta)
	{
	}

	Data::Package::Package(const std::shared_ptr<Data>& item)
		: item(item)
	{
	}

	void Data::Package::Open()
	{
		std::shared_ptr<Item::Data> item = this->item.lock();
		std::shared_ptr<Item::Meta> meta = item->meta;
		//std::shared_ptr<UserSession> session = item->session;
		//auto bag = session->GetComponent<Component::Bag>();
		//auto counter = session->GetComponent<Component::UserCounter>();
		/*
		if(1 > item->count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		for(auto& package : meta->packages)
		{
			if("" != package->item_id)
			{
				bag->Insert(Item::Create(package->item_id, package->count));
			}
			if(Message::CounterType::Invalid != package->counter_type)
			{
				//counter->ChangeCount(package->counter_type, package->count);
			}
		}

		bag->Remove(item->seq, 1);
		*/
	}

	Data::Count::Count(const std::shared_ptr<Meta>& meta)
		: meta(meta)
		, count(1)
	{
	}

	bool Data::Count::Stackable() const
	{
		return 1 < meta.lock()->MaxStack;
	}

	Data::Count::operator int() const
	{
		return count;
	}

	int Data::Count::operator = (int count)
	{
		if(1 < count && false == Stackable())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		this->count = count;
		return this->count;
	}

	int Data::Count::operator += (int count)
	{
		if (false == Stackable())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		this->count += count;
		return this->count;
	}
	
	int Data::Count::operator -= (int count)
	{
		if(this->count < count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		this->count -= count;
		return this->count;
	}

	Data::operator Message::ItemData() const
	{
		Message::ItemData data;
		data.item_index = meta->Index;
		data.item_no = item_no;
		data.item_count = count;
		return data;
	}

	void Manager::Init()
	{
		InitMeta("../MetaData/Item.csv");
		InitMeta("../MetaData/EquipItem.csv");
	}

	void Manager::InitMeta(const std::string& path)
	{
		MetaReader<Meta> reader;
		auto& rows = reader.Read(path);
		for (auto& row : rows)
		{
			if (false == id_metas.insert(std::make_pair(row->Code, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->Code, ")");
			}
			if (false == index_metas.insert(std::make_pair(row->Index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->Index, ")");
			}
		}
	}
	
	std::shared_ptr<Meta> Manager::FindMeta(const std::string& id)
	{
		auto itr = id_metas.find(id);
		if (id_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	std::shared_ptr<Meta> Manager::FindMeta(uint32_t index)
	{
		auto itr = index_metas.find(index);
		if(index_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	GAMNET_BIND_INIT_HANDLER(Item::Manager, Init);	

	std::shared_ptr<Data> Create(const std::string& id, int count)
	{
		std::shared_ptr<Item::Meta> meta = Gamnet::Singleton<Item::Manager>::GetInstance().FindMeta(id);
		if (nullptr == meta)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemID);
		}
		std::shared_ptr<Item::Data> data = meta->CreateInstance();
		data->count = count;
		return data;
	}

	std::shared_ptr<Data> Create(int32_t index, int count)
	{
		std::shared_ptr<Item::Meta> meta = Gamnet::Singleton<Item::Manager>::GetInstance().FindMeta(index);
		if (nullptr == meta)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemIndex);
		}
		std::shared_ptr<Item::Data> data = meta->CreateInstance();
		data->count = count;
		return data;
	}

	void Load(const std::shared_ptr<UserSession>& session)
	{
		auto pUserData = session->GetComponent<Component::UserData>();
		if (nullptr == pUserData)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		auto pSuit = pUserData->pSuit;
		auto pBag = pUserData->pBag;

		pSuit->session = session;

		pBag->session = session;
		pBag->last_item_no = 0;

		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT item_no, item_index, item_count, equip_part "
			"FROM UserItem "
			"WHERE user_no=", session->user_no, " AND delete_yn='N'"
		);

		for (auto& row : rows)
		{
			int32_t itemIndex = row->getInt32("item_index");
			int32_t itemCount = row->getInt32("item_count");

			std::shared_ptr<Item::Data> item = Item::Create(itemIndex, itemCount);
			item->item_no = row->getInt64("item_no");
			pBag->last_item_no = std::max(pBag->last_item_no, item->item_no);

			if (nullptr != item->equip)
			{
				item->equip->part = (Message::EquipItemPartType)row->getInt32("equip_part");
				if (Message::EquipItemPartType::Invalid != item->equip->part)
				{
					pSuit->item_datas[(int)item->equip->part] = item;
					continue;
				}
			}

			pBag->item_datas.insert(std::make_pair(item->item_no, item));
		}
	}

	bool Merge(std::shared_ptr<Data> lhs, std::shared_ptr<Data> rhs)
	{
		if (lhs->meta != rhs->meta)
		{
			return false;
		}

		std::shared_ptr<Meta> meta = lhs->meta;

		if (1 >= meta->MaxStack)
		{
			return false;
		}

		if (lhs->count >= meta->MaxStack)
		{
			return false;
		}

		int count = std::min(meta->MaxStack - lhs->count, (int)rhs->count);
		lhs->count += count;
		rhs->count -= count;

		//auto session = self->session.lock();
		//session->queries->Update("user_item", Gamnet::Format("item_count=", self->stack->count), {
		//	{ "item_seq", self->seq }
		//});
		return true;
	}

	Gamnet::Return<std::shared_ptr<Transaction::Statement>> Equip(const std::shared_ptr<UserSession>& session, int64_t itemNo)
	{
		auto pUserData = session->GetComponent<Component::UserData>();
		if (nullptr == pUserData)
		{
			return (int)Gamnet::ErrorCode::UndefinedError;
		}

		auto pBag = pUserData->pBag;
		auto pItem = pBag->Find(itemNo);
		if (nullptr == pItem)
		{
			return (int)Gamnet::ErrorCode::UndefinedError;
		}

		if (nullptr == pItem->equip)
		{
			return (int)Gamnet::ErrorCode::UndefinedError;
		}

		auto result = pBag->Remove(itemNo, 1);
		if(false == result)
        {
			return (int)Gamnet::ErrorCode::UndefinedError;
        }

		auto pSuit = pUserData->pSuit;
		auto pPrevItem = pSuit->Find(pItem->equip->part);
		if(nullptr != pPrevItem)
		{
            pBag->Insert(pPrevItem);
        }

		pSuit->Equip(pItem);

		std::shared_ptr<Transaction::Statement> statement = std::make_shared<Transaction::Statement>();
		statement->Commit += [pItem](const std::shared_ptr<UserSession>& session, const std::shared_ptr<Transaction::Connection>& db) {
            db->Execute(Gamnet::Format("UPDATE UserItem SET equip_part=", (int)pItem->equip->part, " WHERE user_no=", session->user_no, " AND item_no=", pItem->item_no));
        };
		statement->Rollback += [pItem, pPrevItem](const std::shared_ptr<UserSession>& session) {
			auto pUserData = session->GetComponent<Component::UserData>();
			auto pBag = pUserData->pBag;
			auto pSuit = pUserData->pSuit;

			if (nullptr != pPrevItem)
			{
				pSuit->Equip(pPrevItem);
			}

            pBag->Insert(pItem);
		};
		statement->Sync +=	[pItem](const std::shared_ptr<UserSession>& session) {
            Message::Item::MsgSvrCli_EquipItem_Ntf ntf;
            ntf.item_no = pItem->item_no;
            Gamnet::Network::Tcp::SendMsg(session, ntf, true);
        };
		return statement;
	}

	Gamnet::Return<std::shared_ptr<Transaction::Statement>> InsertIntoBag(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Item::Data>& item)
	{
		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
        std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

		Component::Bag::InsertResult result = pBag->Insert(item);
		if(false == result)
        {
            return result.error_code;
        }

		int64_t user_no = session->user_no;

        std::shared_ptr<Transaction::Statement> statement = std::make_shared<Transaction::Statement>();
		statement->Commit += [result](const std::shared_ptr<UserSession>& session, const std::shared_ptr<Transaction::Connection>& db) {
			auto& insert_items = result.value;

            std::string query = "INSERT INTO UserItem(`user_no`, `item_no`, `item_index`, `item_count`, `equip_part`, `delete_yn`) VALUES ";
			
            for (auto& item : insert_items)
            {
                query += Gamnet::Format("(", session->user_no, ",", item->item_no, ",", item->meta->Index, ",", item->count, ",0,'N'),");
            }

			query.pop_back();

            db->Execute(query);
        };

		statement->Rollback += [result](const std::shared_ptr<UserSession>& session) {
			auto& insert_items = result.value;

			std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
			std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

			for (std::shared_ptr<Item::Data> item : insert_items)
			{
				pBag->Remove(item->item_no, item->count);
			}
        };

		statement->Sync += [result](const std::shared_ptr<UserSession>& session) {
			auto& insert_items = result.value;

			Message::Item::MsgSvrCli_AddItem_Ntf ntf;
			for (std::shared_ptr<Item::Data> item : insert_items)
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
		};

        return statement;
	}

	Gamnet::Return<std::shared_ptr<Transaction::Statement>> RemoveFromBag(const std::shared_ptr<UserSession>& session, int64_t itemNo, int count)
	{
		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

		Gamnet::Return<std::shared_ptr<Item::Data>> result = pBag->Remove(itemNo, count);
		if (false == result)
		{
			return result.error_code;
		}

		auto pItemData = result.value;

		std::shared_ptr<Transaction::Statement> statement = std::make_shared<Transaction::Statement>();
		statement->Commit += [pItemData](const std::shared_ptr<UserSession>& session, const std::shared_ptr<Transaction::Connection>& db) {
            db->Execute(
                Gamnet::Format("UPDATE UserItem SET `item_count` = `", pItemData->count, ",`delete_yn`=", (0 == pItemData->count ? "'Y'" : "'N'"), " WHERE user_no=", session->user_no, " AND item_no=", pItemData->item_no)
            );
        };
		statement->Rollback += [pItemData, count](const std::shared_ptr<UserSession>& session) {
			std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
			std::shared_ptr<Component::Bag> pBag = pUserData->pBag;

			if(nullptr == pBag->Find(pItemData->item_no))
            {
				pBag->Insert(pItemData);
            }
        
			pItemData->count += count;
        };
		statement->Sync += [pItemData](const std::shared_ptr<UserSession>& session) {
            Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
            Message::ItemData msgItemData;
            msgItemData.item_index = pItemData->meta->Index;
            msgItemData.item_no = pItemData->item_no;
            msgItemData.item_count = pItemData->count;
            ntf.item_datas.push_back(msgItemData);

            Gamnet::Network::Tcp::SendMsg(session, ntf, true);
        };
		return statement;
	}
}

