#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include <Gamnet/Network/Tcp/Tcp.h>
#include <IDL/MessageItem.h>
#include "../UserSession.h"
#include "Bag.h"

namespace Item {

	Meta::Equip::Equip()
		: part(Message::EquipItemPartType::Invalid)
		, attack(0)
		, defense(0)
		, speed(0)
	{
		META_CUSTOM(part, Equip::OnPartType);
		META_MEMBER(attack);
		META_MEMBER(defense);
		META_MEMBER(speed);
	}
				 
	void Meta::Equip::OnPartType(Message::EquipItemPartType& member, const std::string& value)
	{
		member = Message::Parse<Message::EquipItemPartType>(value);
	}

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
		META_MEMBER(equip);
		META_MEMBER(price);
		META_MEMBER(expire);
		META_MEMBER(packages);
	}

	void Meta::OnLoad() 
	{
		if(Message::ItemType::Equip == type && nullptr == equip)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(Message::ItemType::Package == type && 0 == packages.size())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
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

	std::shared_ptr<Data> Meta::CreateInstance()
	{
		std::shared_ptr<Data> item = std::make_shared<Data>(shared_from_this());
		if (nullptr != expire)
		{
			item->expire = std::make_shared<Data::Expire>(this->expire);
		}

		if(nullptr != equip)
		{
			item->equip = std::make_shared<Data::Equip>();
		}

		if (0 < packages.size())
		{
			item->package = std::make_shared<Data::Package>(item);
		}

		return item;
	}
	
	Data::Data(const std::shared_ptr<Meta>& meta)
		: meta(meta)
		, seq(0)
		, session(nullptr)
		, equip(nullptr)
		, expire(nullptr)
		, package(nullptr)
		, count(meta)
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

	void Data::Package::Open()
	{
		auto self = item.lock();
		if(1 > self->count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		auto session = self->session;
		auto bag = session->GetComponent<Component::Bag>();

		std::shared_ptr<Meta> meta = item.lock()->meta;
		for(auto& package : meta->packages)
		{
			bag->Insert(Item::Create(package->id, package->count));
		}

		self->count -= 1;

		if(0 < self->count)
		{
			session->queries->Update("user_item",
				Gamnet::Format("item_count=", (int)self->count),
				{
					{ "user_seq", session->user_seq },
					{ "item_seq", self->seq }
				}
			);
		}
		else
		{
			session->queries->Update("user_item",
				Gamnet::Format("item_count=", (int)self->count, ",delete_date=NOW(),delete_yn='Y'"),
				{
					{ "user_seq", session->user_seq },
					{ "item_seq", self->seq }
				}
			);
		}	
		session->on_commit.push_back([session, self]() {
			Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
			ntf.item_datas.push_back(*self);
			Gamnet::Network::Tcp::SendMsg(session, ntf);
		});
	}

	Data::Count::Count(const std::shared_ptr<Meta>& meta)
		: meta(meta)
		, count(1)
	{
	}

	bool Data::Count::Stackable() const
	{
		return 1 < meta.lock()->max_stack;
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
		data.item_index = meta->index;
		data.item_seq = seq;
		data.item_count = count;
		return data;
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

	std::shared_ptr<Data> Create(uint32_t index, int count)
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

		int count = std::min(meta->max_stack - lhs->count, (int)rhs->count);
		lhs->count += count;
		rhs->count -= count;

		//auto session = self->session.lock();
		//session->queries->Update("user_item", Gamnet::Format("item_count=", self->stack->count), {
		//	{ "item_seq", self->seq }
		//});
		return true;
	}
}