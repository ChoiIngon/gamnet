#ifndef GAMNET_DATABASE_REDIS_REDIS_H_
#define GAMNET_DATABASE_REDIS_REDIS_H_

#include "Connection.h"
#include "Subscriber.h"
#include "Transaction.h"

namespace Gamnet { namespace Database { namespace Redis {
		void ReadXml(const char* xml_path);
		bool Connect(int db_type, const char* host, int port);
		ResultSet Execute(int db_type, const std::string& query);
		template <class... ARGS>
		ResultSet Execute(int db_type, ARGS... args)
		{
			return Execute(db_type, Format(args...));
		}

		void Subscribe(int db_type, const std::string& channel, const std::function<void(const std::string& messsage)>& callback);
		bool Publish(int db_type, const std::string& channel, const std::string& message);
		bool Publish(int db_type, const std::string& channel, Json::Value& value);
		void Unsubscribe(int db_type, const std::string& channel);

		bool Set(int db_type, const std::string& key, const std::string& value);
		bool Exists(int db_type, const std::string& key);
		bool Expire(int db_type, const std::string& key, int seconds);
		int64_t TTL(int db_type, const std::string& key);
		int64_t Del(int db_type, const std::string& key);

		std::string Get(int db_type, const std::string& key);
		std::list<std::string> MGet(int db_type, const std::list<std::string>& keys);
		bool SetExpire(int db_type, const std::string& key, const std::string& value, int seconds);

		int64_t ZAdd(int db_type, const std::string& key, const std::string& member, int64_t score);
		int64_t ZUpdate(int db_type, const std::string& key, const std::string& member, int64_t score);
		int64_t ZRem(int db_type, const std::string& key, const std::string& member);
		int64_t ZScore(int db_type, const std::string& key, const std::string& member);
		int64_t ZCountAll(int db_type, const std::string& key);
		int64_t ZIncrBy(int db_type, const std::string& key, const std::string& member, int64_t addScore);

		std::vector<Gamnet::Variant> ZRangeByLex(int db_type, const std::string& key, const std::string& minLex, const std::string& maxLex, int64_t limitCount, int64_t offset = 0);
		std::vector<Gamnet::Variant> ZRangeByScore(int db_type, const std::string& key, int64_t minScore, int64_t maxScore, int64_t limitCount, int64_t offset = 0);
		std::vector<Gamnet::Variant> ZRevRangeByScore(int db_type, const std::string& key, int64_t minScore, int64_t maxScore, int64_t limitCount = 0, int64_t offset = 0);

		// 0 based rank. lowest score rank is 0.
		// If member does not exist in the sorted set or key does not exist, returns -1.
		int64_t ZRank(int db_type, const std::string& key, const std::string& member);
		int64_t ZRevRank(int db_type, const std::string& key, const std::string& member);

		int64_t HDel(int db_type, const std::string& key, const std::string& field);
		int64_t HSet(int db_type, const std::string& key, const std::string& field, const std::string& value);
		bool HMSet(int db_type, const std::string& key, const std::map<std::string, Variant>& field_values);
		std::map<std::string, Variant> HMGet(int db_type, const std::string& key, const std::list<std::string>& fields);

		bool LPush(int db_type, const std::string& key, const std::string& value);
		int64_t LRem(int db_type, const std::string& key, const std::string& value, int count);
		std::list<std::string> LRange(int db_type, const std::string& key, int start, int end);

		bool SAdd(int db_type, const std::string& key, const std::string& member);
		int64_t SCard(int db_type, const std::string& key);
		int64_t SRem(int db_type, const std::string& key, const std::string& member);
		std::list<Variant> SMembers(int db_type, const std::string& key);
		std::list<Variant> SRandMembers(int db_type, const std::string& key, int64_t limitCount = 1);
}}}

#endif /* DATABASE_H_ */

