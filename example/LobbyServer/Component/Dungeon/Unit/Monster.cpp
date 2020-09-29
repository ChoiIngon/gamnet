#include "Monster.h"
#include "MonsterBehaviour.h"
#include <Gamnet/Library/Singleton.h>
#include <idl/MessageCommon.h>

namespace Component { namespace Monster {
Meta::Meta()
	: id("")
	, index(0)
	, name("")
	, behaviour(nullptr)
{
	GAMNET_META_MEMBER(id);
	GAMNET_META_MEMBER(index);
	GAMNET_META_MEMBER(name);
	GAMNET_META_CUSTOM(behaviour, Meta::OnBehaviourPath);
}

void Meta::OnBehaviourPath(std::shared_ptr<BehaviourTree>& behaviour, const std::string& value)
{
	behaviour = std::make_shared<BehaviourTree>();
	BehaviourTree::Meta meta;
	meta.BindAction<RiseFromChair>("RiseFromChair");
	meta.BindAction<MoveToVendingMachine>("MoveToVendingMachine");
	meta.BindAction<BuyTea>("BuyTea");
	meta.BindAction<BuyCoffee>("BuyCoffee");
	meta.BindAction<ReturnToChair>("ReturnToChair");
	behaviour->root = meta.ReadXml("../MetaData/" + value);
}

void Manager::Init()
{
	Gamnet::MetaReader<Meta> reader;
	auto& rows = reader.Read("../MetaData/Monster.csv");
	for (auto& row : rows)
	{
		if(false == index_metas.insert(std::make_pair(row->index, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster index(", row->index, ")");
		}
		if(false == id_metas.insert(std::make_pair(row->id, row)).second)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate monster id(", row->id, ")");
		}
	}
}

std::shared_ptr<Meta> Manager::FindMeta(const std::string& id)
{
	auto itr = id_metas.find(id);
	if(id_metas.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<Meta> Manager::FindMeta(uint32_t index)
{
	auto itr = index_metas.find(index);
	if (index_metas.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<Data> Manager::CreateInstance(const std::string& id)
{
	std::shared_ptr<Meta> meta = FindMeta(id);
	if(nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Data> Manager::CreateInstance(uint32_t index)
{
	std::shared_ptr<Meta> meta = FindMeta(index);
	if (nullptr == meta)
	{
		return nullptr;
	}

	return CreateInstance(meta);
}

std::shared_ptr<Data> Manager::CreateInstance(const std::shared_ptr<Meta>& meta)
{
	std::shared_ptr<Data> data = std::make_shared<Data>();
	data->meta = meta;
	return data;
}

}}
GAMNET_BIND_INIT_HANDLER(Component::Monster::Manager, Init);