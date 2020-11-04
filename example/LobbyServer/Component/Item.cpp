#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <Gamnet/Network/Tcp/Tcp.h>
#include <IDL/MessageItem.h>
#include "../UserSession.h"
#include "Bag.h"

namespace Item {

	Meta::Price::Price()
		: type(Message::CounterType::Invalid)
		, value(0)
	{
		META_CUSTOM(type, Price::OnPriceType);
		META_MEMBER(value);
	}

	void Meta::Price::OnPriceType(Message::CounterType& member, const std::string& value)
	{
		member = Message::Parse<Message::CounterType>(value);
	}

	Meta::Expire::Expire()
		: trigger_type(TriggerType::None)
		, expire_type(ExpireType::Infinite)
		, time(0)
		, date(0)
	{
		META_CUSTOM(trigger_type, Expire::OnTriggerType);
		META_CUSTOM(expire_type, Expire::OnExpireType);
		META_MEMBER(time);
		META_MEMBER(date);
	}

	void Meta::Expire::OnTriggerType(TriggerType& member, const std::string& value)
	{
		member = TriggerType::None;

		if ("OnCreate" == value)
		{
			member = TriggerType::OnCreate;
			return;
		}
		else if ("OnEquip" == value)
		{
			member = TriggerType::OnEquip;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	void Meta::Expire::OnExpireType(ExpireType& expireType, const std::string& value)
	{
		expireType = ExpireType::Infinite;

		if ("Infinite" == value)
		{
			expireType = ExpireType::Infinite;
			return;
		}
		else if ("DueDate" == value)
		{
			expireType = ExpireType::DueDate;
			return;
		}
		else if ("Period" == value)
		{
			expireType = ExpireType::Period;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	Meta::Package::Package()
		: count(0)
	{
		META_MEMBER(id);
		META_MEMBER(count);
	}
	
	Meta::Meta()
		: id("")
		, index(0)
		, type(Message::ItemType::Invalid)
		, grade(0)
		, max_stack(0)
	{
		META_MEMBER(id);
		META_MEMBER(index);
		META_CUSTOM(type, Meta::OnItemType);
		META_MEMBER(grade);
		META_MEMBER(max_stack);
		META_MEMBER(price);
		META_MEMBER(expire);
		META_MEMBER(packages);
	}

	void Meta::OnLoad() 
	{
		if(nullptr != expire)
		{
			if(Expire::TriggerType::None == expire->trigger_type || Expire::ExpireType::Infinite == expire->expire_type)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
		}
	}
	void Meta::OnItemType(Message::ItemType& member, const std::string& value)
	{
		member = Message::Parse<Message::ItemType>(value);
	}

	std::shared_ptr<Data> Meta::CreateInstance(const std::shared_ptr<UserSession>& session)
	{
		std::shared_ptr<Data> item = std::make_shared<Data>(session, shared_from_this());
		if (nullptr != expire)
		{
			item->expire = std::make_shared<Data::Expire>(this->expire);
		}

		if (0 < packages.size())
		{
			item->package = std::make_shared<Data::Package>(item);
		}

		return item;
	}
	
	Data::Data(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Meta>& meta)
		: seq(0)
		, meta(meta)
		, equip(nullptr)
		, expire(nullptr)
		, package(nullptr)
		, session(session)
	{
	}

	Data::Expire::Expire(const std::shared_ptr<Meta::Expire>& meta)
		: meta(meta)
		, expire_date(Gamnet::Time::DateTime::MaxValue)
	{
	}

	void Data::Expire::TriggerExpire(Meta::Expire::TriggerType triggerType)
	{
		if (Meta::Expire::ExpireType::Infinite == meta->expire_type)
		{
			return;
		}

		if (triggerType != meta->trigger_type)
		{
			return;
		}

		if (Gamnet::Time::DateTime::MaxValue > expire_date) // already start
		{
			return;
		}

		if (Meta::Expire::ExpireType::DueDate == meta->expire_type)
		{
			expire_date = meta->date;
		}
		else
		{
			expire_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + meta->time);
		}
	}

	void Data::Expire::SetDate(const Gamnet::Time::DateTime& date)
	{
		expire_date = date;
	}

	const Gamnet::Time::DateTime& Data::Expire::GetDate() const
	{
		return expire_date;
	}

	Data::Package::Package(const std::shared_ptr<Data>& item)
		: item(item)
	{
	}

	void Data::Package::Use()
	{
		auto self = item.lock();
		if(0 >= self->count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		auto session = self->session.lock();
		auto bag = session->GetComponent<Component::Bag>();

		std::shared_ptr<Meta> meta = item.lock()->meta;
		for(auto& package : meta->packages)
		{
			std::shared_ptr<Data> item = Gamnet::Singleton<Manager>::GetInstance().CreateInstance(session, package->id, package->count);
			bag->Insert(item);
		}
	}

	const Gamnet::Time::DateTime& Data::GetExpireDate() const
	{
		if(nullptr == expire)
		{
			return Gamnet::Time::DateTime::MaxValue;
		}

		return expire->GetDate();
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
			if (false == id_metas.insert(std::make_pair(row->id, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->id, ")");
			}
			if (false == index_metas.insert(std::make_pair(row->index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->index, ")");
			}
		}
	}
	
	std::shared_ptr<Item::Data> Manager::CreateInstance(const std::shared_ptr<UserSession>& session, const std::string& id, int count)
	{
		std::shared_ptr<Item::Meta> meta = FindMeta(id);
		if(nullptr == meta)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemID);
		}
		std::shared_ptr<Item::Data> data = meta->CreateInstance(session);
		data->count = count;
		return data;
	}

	std::shared_ptr<Item::Data> Manager::CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t index, int count)
	{
		std::shared_ptr<Item::Meta> meta = FindMeta(index);
		if (nullptr == meta)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemIndex);
		}
		std::shared_ptr<Item::Data> data = meta->CreateInstance(session);
		data->count = count;
		return data;
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

	bool Merge(std::shared_ptr<Data> lhs, std::shared_ptr<Data> rhs)
	{
		if (lhs->meta != rhs->meta)
		{
			return false;
		}

		std::shared_ptr<Meta> meta = lhs->meta;

		if (1 >= meta->max_stack)
		{
			return false;
		}

		if (lhs->count >= meta->max_stack)
		{
			return false;
		}

		if (lhs->GetExpireDate() != rhs->GetExpireDate())
		{
			return false;
		}

		int count = std::min(meta->max_stack - lhs->count, rhs->count);
		lhs->count += count;
		rhs->count -= count;

		//auto session = self->session.lock();
		//session->queries->Update("user_item", Gamnet::Format("item_count=", self->stack->count), {
		//	{ "item_seq", self->seq }
		//});
		return true;
	}
}