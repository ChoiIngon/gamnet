#ifndef _ITEM_H_
#define _ITEM_H_

#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>
#include "../Util/MetaData.h"

class UserSession;
namespace Item 
{
	class Data;
	struct Meta : public std::enable_shared_from_this<Meta>, public MetaData
	{
		struct Equip : public MetaData
		{
			Equip();

			Message::EquipItemPartType part;
			int attack;
			int defense;
			int speed;

		private :
			void OnPartType(Message::EquipItemPartType& member, const std::string& value);
		};

		struct Price : public MetaData
		{
			Price();

			Message::CounterType type;
			int value;

		private :
			void OnPriceType(Message::CounterType& member, const std::string& value);
		};

		struct Expire : public MetaData
		{
			enum class TriggerType
			{
				None = 0,
				OnCreate = 1,
				OnEquip = 2
			};

			enum class ExpireType
			{
				Infinite = 0,
				DueDate = 1,
				Period = 2
			};

			Expire();

			TriggerType trigger_type;
			ExpireType expire_type;
			int64_t time;
			Gamnet::Time::DateTime date;

		private :
			void OnTriggerType(TriggerType& triggerType, const std::string& value);
			void OnExpireType(ExpireType& expireType, const std::string& value);
		};

		struct Package : public MetaData
		{
			Package();

			std::string item_id;
			Message::CounterType counter_type;
			int		 count;

		private :
			void OnCounterType(Message::CounterType& member, const std::string& value);
		};

		Meta();

		virtual void OnLoad() override;
		
		std::shared_ptr<Data> CreateInstance();

		std::string			id;
		uint32_t			index;
		Message::ItemType	type;
		int					grade;
		int					max_stack;

		std::shared_ptr<Equip> equip;
		std::shared_ptr<Price> price;
		std::shared_ptr<Expire> expire;
		std::vector<std::shared_ptr<Package>> packages;
	private :
		void OnItemType(Message::ItemType& member, const std::string& value);
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
		
		class Expire
		{
		public :
			Expire(const std::shared_ptr<Meta::Expire>& meta);

			void TriggerExpire(Meta::Expire::TriggerType triggerType);
			
			void SetDate(const Gamnet::Time::DateTime& date);
			const Gamnet::Time::DateTime& GetDate() const;
		private :
			const std::shared_ptr<Meta::Expire> meta;
			Gamnet::Time::DateTime expire_date;
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
		
		const Gamnet::Time::DateTime& GetExpireDate() const;
	public :
		const std::shared_ptr<Meta>	meta;
		int64_t						item_no;
		std::shared_ptr<Equip>		equip;
		std::shared_ptr<Expire>		expire;
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
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};

	std::shared_ptr<Data> Create(const std::string& id, int count);
	std::shared_ptr<Data> Create(uint32_t index, int count);

	bool Merge(std::shared_ptr<Data> lhs, std::shared_ptr<Data> rhs);
};

#endif
