#ifndef _ITEM_H_
#define _ITEM_H_

#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>
#include "../../Util/MetaData.h"
#include "../../Util/Transaction.h"

class UserSession;

namespace Item 
{
	class Data;
	struct Meta : public std::enable_shared_from_this<Meta>, public MetaData
	{
		struct Equip : public MetaData
		{
			Equip();

			Message::EquipItemPartType Part;
			int Attack;
			int Defense;
			int Speed;

		private :
			void OnPartType(Message::EquipItemPartType& part, const std::string& value);
		};

		struct Price : public MetaData
		{
			Price();

			Message::CounterType Type;
			int Value;

		private :
			void OnPriceType(Message::CounterType& type, const std::string& value);
		};

		struct Expire : public MetaData
		{
			enum class ETriggerType
			{
				None = 0,
				OnCreate = 1,
				OnEquip = 2
			};

			enum class EExpireType
			{
				Infinite = 0,
				DueDate = 1,
				Period = 2
			};

			typedef Gamnet::Time::DateTime DateTime;

			Expire();

			ETriggerType	TriggerType;
			EExpireType		ExpireType;
			int64_t			Time;
			DateTime		Date;

		private :
			void OnTriggerType(ETriggerType& triggerType, const std::string& value);
			void OnExpireType(EExpireType& expireType, const std::string& value);
		};

		struct Package : public MetaData
		{
			Package();

			std::string Code;
			int			Count;
		};

		Meta();

		virtual void OnLoad() override;
		
		std::shared_ptr<Data> CreateInstance();

		std::string			Code;
		uint32_t			Index;
		Message::ItemType	Type;
		int					Grade;
		int					MaxStack;

		std::shared_ptr<Equip> Equip;
		std::shared_ptr<Price> Price;
		std::shared_ptr<Expire> Expire;
		std::vector<std::shared_ptr<Package>> Package;
	private :
		void OnItemType(Message::ItemType& typeType, const std::string& value);
	};
			   
	class Data
	{
	public :
		class Equip
		{
		public :
			Equip();

			Message::EquipItemPartType part;
		}; 
		
		class Package
		{
		public :
			Package(const std::shared_ptr<Data>& item);
			void Open();
		private:
			const std::weak_ptr<Data> item;
		};
		
		struct Count
		{
			Count(const std::shared_ptr<Meta>& meta);
			bool Stackable() const;

			operator int() const;
			int operator = (int count);
			int operator += (int count);
			int operator -= (int count);
		private :
			std::weak_ptr<Meta> meta;
			int count;
		};

		Data(const std::shared_ptr<Meta>& meta);
		
	public :
		const std::shared_ptr<Meta>	meta;
		int64_t						item_no;
		std::shared_ptr<Equip>		equip;
		std::shared_ptr<Package>	package;
		Count						count;
		
		operator Message::ItemData() const;
	};

	class Manager
	{
	public :
		void Init();
		
		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
	private :
		void InitMeta(const std::string& path);
		std::map<int32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};

	std::shared_ptr<Data> Create(const std::string& id, int count);
	std::shared_ptr<Data> Create(int32_t index, int count);

	void Load(const std::shared_ptr<UserSession>& session);
	bool Merge(std::shared_ptr<Data> lhs, std::shared_ptr<Data> rhs);
	
	Gamnet::Return<std::shared_ptr<Transaction::Statement>> InsertIntoBag(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Item::Data>& item);
	Gamnet::Return<std::shared_ptr<Transaction::Statement>> RemoveFromBag(const std::shared_ptr<UserSession>& session, int64_t itemNo, int count);
	Gamnet::Return<std::shared_ptr<Transaction::Statement>> Equip(const std::shared_ptr<UserSession>& session, int64_t itemNo);
}

#endif
