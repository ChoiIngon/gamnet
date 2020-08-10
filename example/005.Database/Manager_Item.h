#ifndef _MANAGER_ITEM_H_
#define _MANAGER_ITEM_H_

#include <Gamnet/Library/MetaData.h>

struct ItemMeta : public Gamnet::MetaData
{
	struct Stat
	{
		int stat_id;
		float value;
	};

	enum class Type
	{
		Invalid,
		ItemType_1,
		ItemType_2
	};
	int	item_id;
	std::string name;
	Type type;
	int grade;
	int max_stack;
	int price;
	Stat stat;

	ItemMeta();

	void OnType(Type& member, const std::string& value);

	virtual bool OnLoad() override;
};

class Manager_Item
{
	Gamnet::MetaReader<ItemMeta> itemmeta_reader;
	std::map<int, std::shared_ptr<ItemMeta>> item_metas;
	std::vector<std::shared_ptr<ItemMeta>> random;
public :
	void Init();

	std::shared_ptr<ItemMeta> FindMeta(int itemID);
	std::shared_ptr<ItemMeta> RandMeta() const;
};

#endif
