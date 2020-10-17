#ifndef _ITEM_H_
#define _ITEM_H_

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>
#include "../Util/MetaData.h"

class UserSession;
namespace Item 
{
	class Data;
	struct Meta : public std::enable_shared_from_this<Meta>, public MetaData
	{
		struct Price : public MetaData
		{
			Price();

			void OnPriceType(Message::CounterType& member, const std::string& value);

			Message::CounterType type;
			int value;
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

			void OnTriggerType(TriggerType& triggerType, const std::string& value);
			void OnExpireType(ExpireType& expireType, const std::string& value);

			TriggerType trigger_type;
			ExpireType expire_type;
			int64_t time;
			Gamnet::Time::DateTime date;
		};

		struct Package : public MetaData
		{
			Package();

			std::string id;
			int		 count;
		};

		Meta();

		virtual void OnLoad() override;
		void OnItemType(Message::ItemType& member, const std::string& value);
		std::shared_ptr<Data> CreateInstance(const std::shared_ptr<UserSession>& session);

		std::string			id;
		uint32_t			index;
		Message::ItemType	type;
		int					grade;
		int					max_stack;

		std::shared_ptr<Price> price;
		std::shared_ptr<Expire> expire;
		std::vector<std::shared_ptr<Package>> packages;
	};
			   
	class Data
	{
	public :
		class Equip
		{
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
			void Use();
		private:
			const std::weak_ptr<Data> item;
		};
		
		class Stack
		{
		public :
			Stack(const std::shared_ptr<Data>& item);
			void Merge(const std::shared_ptr<Data>& other);

		public :
			int count;
		private:
			const std::weak_ptr<Data> item;
		};

		Data(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Meta>& meta);
		
		// Property Functions..
		int GetCount() const;
		const Gamnet::Time::DateTime& GetExpireDate() const;

		// Event Functions..
		void OnBag();
		void OnEquip();
		
		uint64_t					seq;
		const std::shared_ptr<Meta>	meta;
		std::shared_ptr<Equip>		equip;
		std::shared_ptr<Expire>		expire;
		std::shared_ptr<Package>	package;
		std::shared_ptr<Stack>		stack;
		
		operator Message::ItemData() const
		{
			Message::ItemData data;
			data.item_id = meta->index;
			data.item_type = meta->type;
			data.item_seq = seq;
			data.item_count = 0;
			return data;
		}
	private :
		std::weak_ptr<UserSession> session;
	};

	class Manager
	{
	public :
		void Init();

		std::shared_ptr<Data> CreateInstance(const std::shared_ptr<UserSession>& session, const std::string& id, int count);
		std::shared_ptr<Data> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t index, int count);

		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
	private :
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};
};

#endif
