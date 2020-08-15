#ifndef _ITEM_H_
#define _ITEM_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <Gamnet/Library/MetaData.h>
#include <Gamnet/Library/Time/Time.h>
#include "../../../idl/MessageCommon.h"

struct ItemMeta : public Gamnet::MetaData
{
	ItemMeta();

	void OnItemType(ItemType& member, const std::string& value);
	void OnPriceType(CounterType& member, const std::string& value);

	virtual bool OnLoad() override;

	uint32_t	item_id;
	ItemType	item_type;
	int			grade;
	int			max_stack;

	CounterType price_type;
	int price;
	time_t expire_time;
	Gamnet::Time::DateTime expire_date;
	std::vector<uint32_t> packages;
};

struct ItemData
{
	uint64_t item_seq;
	
	class Equip
	{
	};

	struct Expire
	{
		Gamnet::Time::DateTime expire_date;
	};

	struct Package
	{
		std::vector<std::shared_ptr<ItemMeta>> item_metas;
	};

	struct Stack
	{
		int32_t cur_count;
		int32_t max_count;
	};

	std::shared_ptr<ItemMeta>	meta;
	std::shared_ptr<Expire>		expire;
	std::shared_ptr<Package>	package;
	std::shared_ptr<Stack>		stack;
};

class UserSession;
class Manager_Item
{
	Gamnet::MetaReader<ItemMeta> itemmeta_reader;
	std::map<int, std::shared_ptr<ItemMeta>> item_metas;
	std::vector<std::shared_ptr<ItemMeta>> random;

public :
	void Init();
	std::shared_ptr<ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemid, int32_t count);
	std::shared_ptr<ItemMeta> FindMeta(int itemID);
	std::shared_ptr<ItemMeta> RandMeta() const;
};

namespace Item {
	std::shared_ptr<ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemid, int32_t count);
};
#endif
