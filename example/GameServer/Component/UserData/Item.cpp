#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <Gamnet/Network/Tcp/Tcp.h>
#include <IDL/MessageItem.h>
#include "../../UserSession.h"
#include "Bag.h"
#include "../UserCounter.h"

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
}

