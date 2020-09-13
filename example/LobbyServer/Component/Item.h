#ifndef _ITEM_H_
#define _ITEM_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <Gamnet/Library/MetaData.h>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>

class UserSession;
namespace Component {

	struct ItemData;
	struct ItemMeta : public std::enable_shared_from_this<ItemMeta>
	{
		struct SellMeta
		{
			Message::CounterType price_type;
			int price;
		};

		struct ExpireMeta
		{
			enum Type
			{
				None = 0,
				Bag = 1,
				Equip = 2
			};
			Type expire_type;
			int64_t expire_time;
			Gamnet::Time::DateTime expire_date;
		};

		struct PackageMeta
		{
			uint32_t item_id;
			int		 item_count;
		};

		ItemMeta();
		std::shared_ptr<ItemData> CreateInstance(const std::shared_ptr<UserSession>& session);
		uint32_t	item_id;
		Message::ItemType	item_type;
		int			grade;
		int			max_stack;
		std::shared_ptr<SellMeta> sell_meta;
		std::shared_ptr<ExpireMeta> expire_meta;
		std::vector<std::shared_ptr<PackageMeta>> package_metas;
	};

struct ItemData
{
	class Equip
	{
	};

	struct Expire
	{
		Expire(const std::shared_ptr<ItemData>& item);
		void StartExpire();

	private :
		const std::weak_ptr<ItemData> item_data;
	};

	struct Package
	{
		Package(const std::shared_ptr<ItemData>& item);
		void Use();

	private:
		const std::weak_ptr<ItemData> item_data;
	};

	struct Stack
	{
		Stack(const std::shared_ptr<ItemData>& item);

		void Merge(const std::shared_ptr<ItemData>& other);
	private:
		const std::weak_ptr<ItemData> item_data;
	};

	ItemData(const std::shared_ptr<UserSession>& session, const std::shared_ptr<ItemMeta>& meta);

	uint64_t item_seq;
	int32_t item_count;
	Gamnet::Time::DateTime expire_date;

	const std::shared_ptr<ItemMeta>	meta;
	std::shared_ptr<Expire>		expire;
	std::shared_ptr<Package>	package;
	std::shared_ptr<Stack>		stack;

	operator Message::ItemData() const
	{
		Message::ItemData data;
		data.item_id = meta->item_id;
		data.item_type = meta->item_type;
		data.item_seq = item_seq;
		data.item_count = item_count;
		return data;
	}
private :
	std::weak_ptr<UserSession> session;
};

class Manager_Item
{
	std::map<int, std::shared_ptr<ItemMeta>> item_metas;
public :
	void Init();
	std::shared_ptr<ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemid, int32_t count);
	std::shared_ptr<ItemMeta> FindMeta(int itemID);
};
}
namespace Item {
	std::shared_ptr<Component::ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemid, int32_t count);
};

#endif
