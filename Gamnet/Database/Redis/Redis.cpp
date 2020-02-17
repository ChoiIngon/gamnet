#include "Redis.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "Subscriber.h"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Database { namespace Redis {

	static std::map<int, Connection::ConnectionInfo> connectionInfos;
	static std::map<int, std::shared_ptr<Subscriber>> subscribers;

	std::string CheckString(const std::string& value)
	{
		const static std::string from = "'";
		const static std::string to = "\\'";

		std::string result = value;
		size_t start_pos = 0;
		while ((start_pos = result.find(from, start_pos)) != std::string::npos)
		{
			result.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}

		return result;
	}

	bool IsResultOK(const std::string& result)
	{
		return "ok" == boost::algorithm::to_lower_copy(result);
	}

	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		auto database_ = ptree_.get_child("server");
		for (const auto& elmt : database_)
		{
			if ("redis" != elmt.first)
			{
				continue;
			}
			int id = elmt.second.get<int>("<xmlattr>.id");
			int port = elmt.second.get<int>("<xmlattr>.port");
			const std::string host = elmt.second.get<std::string>("<xmlattr>.host");
			if (false == Connect(id, host.c_str(), port))
			{
				throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "database connect  fail(id:", id, ", host:", host, ", port:", port, ")");
			}
		}
	}
	
	bool Connect(int db_type, const char* host, int port)
	{
		Connection::ConnectionInfo connInfo;
		connInfo.host = host;
		connInfo.port = port;

		if(false == connectionInfos.insert(std::make_pair(db_type, connInfo)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidDatabaseNum, "duplicated db_type:", db_type);
		}

		std::shared_ptr<Network::Link> subscriber = Singleton<SubscriberManager>::GetInstance().Create();
		subscriber->Connect(connInfo.host.c_str(), connInfo.port, 5);
		subscribers.insert(std::make_pair(db_type, std::static_pointer_cast<Subscriber>(subscriber)));
		return Singleton<ConnectionPool<Connection>>::GetInstance().Connect(db_type, connInfo);
	}

	ResultSet Execute(int db_type, const std::string& query)
	{
		ResultSet res;
		try
		{
			std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
			if (nullptr != conn)
			{
				res.impl_ = conn->Execute(query);
			}
		}
		catch (const std::exception& ex)
		{
			LOG(WRN, "redis query failed(error:", ex.what(), ", query:", query, ")");
			res.impl_ = nullptr;
		}
		return res;
	}

	void Subscribe(int db_type, const std::string& channel, const std::function<void(const std::string& message)>& callback)
	{
		auto itr_ConnectionInfo = connectionInfos.find(db_type);
		if(itr_ConnectionInfo == connectionInfos.end())
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidDatabaseNum, "can not find database connection info(db_type:", db_type, ")");
		}
		
		auto itr_Subscriber = subscribers.find(db_type);
		if(subscribers.end() == itr_Subscriber)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidDatabaseNum, "can not find database connection info(db_type:", db_type, ")");
		}
		std::shared_ptr<Subscriber> subscriber = itr_Subscriber->second;
		subscriber->Subscribe(channel, callback);
	}

	bool Publish(int db_type, const std::string& channel, const std::string& message)
	{
		auto ret = Execute(db_type, "PUBLISH ", CheckString(channel), " '", CheckString(message), "'");
		if (true == ret.empty())
		{
			return false;
		}
		return true;
	}

	bool Publish(int db_type, const std::string& channel, Json::Value& value)
	{
		Json::FastWriter writer;
		writer.omitEndingLineFeed();
		std::string message = writer.write(value);
		return Publish(db_type, channel, message);
	}

	void Unsubscribe(int db_type, const std::string& channel)
	{
		auto itr_Subscriber = subscribers.find(db_type);
		if(itr_Subscriber == subscribers.end())
		{
			return;
		}

		std::shared_ptr<Subscriber> subscriber = itr_Subscriber->second;
		subscriber->Unsubscribe(channel);
	}

	// EXISTS key [key ...]
	// Time complexity: O(1)
	// Returns : 
	bool Exists(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "EXISTS '", CheckString(key), "'");

		if (false == ret.empty())
		{
			return ret.value().asBool();
		}
		return false;
	}

	// EXPIRE key seconds
	// Time complexity: O(1)
	bool Expire(int db_type, const std::string& key, int seconds)
	{
		auto ret = Execute(db_type, "EXPIRE '", CheckString(key), "' ", seconds);
		if (false == ret.empty())
		{
			return ret.value().asBool();
		}
		return false;
	}

	// TTL key
	// Time complexity: O(1)
	int64_t TTL(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "TTL '", CheckString(key), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	bool Set(int db_type, const std::string& key, const std::string& value)
	{
		auto ret = Execute(db_type, "SET ", CheckString(key), " \"", CheckString(value), "\"");
		if(true == ret.empty())
		{
			return false;
		}

		if("OK" != ret.value().asString())
		{
			return false;
		}

		return true;
	}

	// DEL key [key ...]
	// Time complexity: O(N)
	//  N : 삭제 할 멤버 수
	//  -- list, set, sorted set, hash 일 경우 요소 개수만큼
	int64_t Del(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "DEL '", CheckString(key), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// GET key
	// Time complexity: O(1)
	std::string Get(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "GET '", CheckString(key), "'");
		if (false == ret.empty())
		{
			return ret.value().asString();
		}
		return "";
	}
	
	// MGET key [key ...]
	// Time complexity: O(N)
	//  N : 검색 할 수
	std::list<std::string> MGet(int db_type, const std::list<std::string>& keys)
	{
		std::string searchKeys;
		for (const auto& key : keys)
		{
			searchKeys += Format(" '", CheckString(key), "'");
		}

		auto ret = Execute(db_type, "MGET ", searchKeys);

		std::list<std::string> result;
		for (const auto& row : ret)
		{
			result.push_back(row.asString());
		}

		return result;
	}

	// SETEX key seconds value
	// Time complexity: O(1)
	bool SetExpire(int db_type, const std::string& key, const std::string& value, int seconds)
	{
		auto ret = Execute(db_type, "SETEX '", CheckString(key), "' ", seconds, " '", CheckString(value), "'");
		if (false == ret.empty())
		{
			return IsResultOK(ret.value().asString());
		}
		return false;
	}

	// ZADD key [NX|XX] [CH] [INCR] score member [score member ...]
	// Time complexity: O(log(N))
	//  N : 목록의 멤버 수
	int64_t ZAdd(int db_type, const std::string& key, const std::string& member, int64_t score)
	{
		auto ret = Execute(db_type, "ZADD '", CheckString(key), "' ", score, " '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	int64_t ZUpdate(int db_type, const std::string& key, const std::string& member, int64_t score)
	{
		// XX: Only update elements that already exist. Never add elements.
		auto ret = Execute(db_type, "ZADD '", CheckString(key), "' XX ", score, " '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// ZREM key member [member ...]
	// Time complexity: O(M*log(N))
	//  N : 목록의 멤버 수
	//  M : 삭제할 멤버 수
	int64_t ZRem(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "ZREM '", CheckString(key), "' '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// ZSCORE key member
	// Time complexity: O(1)
	int64_t ZScore(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "ZSCORE '", CheckString(key), "' '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// ZCOUNT key min max
	// Time complexity: O(log(N))
	//  N : 목록의 멤버 수
	int64_t ZCountAll(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "ZCOUNT '", CheckString(key), "' -inf +inf");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// ZINCRBY key increment member
	// Time complexity: O(log(N))
	//  N : 목록의 멤버 수
	int64_t ZIncrBy(int db_type, const std::string& key, const std::string& member, int64_t addScore)
	{
		auto ret = Execute(db_type, "ZINCRBY '", CheckString(key), "' ", addScore, " '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// ZRANGEBYLEX key min max [LIMIT offset count]
	// Time complexity: O(log(N)+M)
	//  N : 목록의 멤버 수
	//  M : 검색된 멤버 수
	std::vector<Gamnet::Variant> ZRangeByLex(int db_type, const std::string& key, const std::string& minLex, const std::string& maxLex, int64_t limitCount, int64_t offset)
	{
		std::string limit;
		if (0 < limitCount)
		{
			limit = Format("LIMIT ", offset, " ", limitCount);
		}

		auto ret = Execute(db_type, "ZRANGEBYLEX '", CheckString(key), "' '", CheckString(minLex), "' '", CheckString(maxLex), "' ", limit);

		std::vector<Gamnet::Variant> result;
		if (0 == ret.size())
		{
			return result;
		}

		result.resize(ret.size());
		int i = 0;
		for (const auto& row : ret)
		{
			result[i++] = row.asString();
		}
		return result;
	}

	// ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
	// Time complexity: O(log(N)+M)
	//  N : 목록의 멤버 수
	//  M : 검색된 멤버 수
	std::vector<Gamnet::Variant> ZRangeByScore(int db_type, const std::string& key, int64_t minScore, int64_t maxScore, int64_t limitCount, int64_t offset)
	{
		std::string limit;
		if (0 < limitCount)
		{
			limit = Format("LIMIT ", offset, " ", limitCount);
		}

		auto ret = Execute(db_type, "ZRANGEBYSCORE '", CheckString(key), "' ", minScore, " ", maxScore, " ", limit);

		std::vector<Gamnet::Variant> result;
		if (0 == ret.size())
		{
			return result;
		}

		result.resize(ret.size());
		int i = 0;
		for (const auto& row : ret)
		{
			result[i++] = row.asString();
		}
		return result;
	}

	// ZREVRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
	// Time complexity: O(log(N)+M)
	//  N : 목록의 멤버 수
	//  M : 검색된 멤버 수
	std::vector<Gamnet::Variant> ZRevRangeByScore(int db_type, const std::string& key, int64_t minScore, int64_t maxScore, int64_t limitCount, int64_t offset)
	{
		std::string limit;
		if (0 < limitCount)
		{
			limit = Format("LIMIT ", offset, " ", limitCount);
		}

		std::vector<Gamnet::Variant> result;
		auto ret = Execute(db_type, "ZREVRANGEBYSCORE '", CheckString(key), "' ", maxScore, " ", minScore, " ", limit);
		if(0 == ret.size())
		{
			return result;
		}

		result.resize(ret.size());
		int i = 0;
		for (const auto& row : ret)
		{
			result[i++] = row.asString();
		}
		return result;
	}

	int64_t ZRank(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "ZRANK ", CheckString(key), " ", CheckString(member));
		if(false == ret.value().isInt64())
		{
			return -1;
		}
		return ret.value().asInt64();
	}

	int64_t ZRevRank(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "ZREVRANK ", CheckString(key), " ", CheckString(member));
		if (false == ret.value().isInt64())
		{
			return -1;
		}
		return ret.value().asInt64();
	}
	// HDEL key field [field ...]
	// Time complexity: O(N)
	//  N : 삭제 할 수
	int64_t HDel(int db_type, const std::string& key, const std::string& field)
	{
		auto ret = Execute(db_type, "HDEL '", CheckString(key), "' '", CheckString(field), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// HSET key field value
	// Time complexity: O(1)
	int64_t HSet(int db_type, const std::string& key, const std::string& field, const std::string& value)
	{
		auto ret = Execute(db_type, "HSET '", CheckString(key), "' '", CheckString(field), "' '", CheckString(value), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// HMSET key field value [field value ...]
	// Time complexity: O(N)
	//  N : 입력 할 필드 수
	bool HMSet(int db_type, const std::string& key, const std::map<std::string, Variant>& field_values)
	{
		std::string searchKeys;
		for (const auto& pair : field_values)
		{
			searchKeys += Format(" '", CheckString(pair.first), "' '", CheckString(pair.second), "'");
		}

		auto ret = Execute(db_type, "HMSET '", CheckString(key), "'", searchKeys);
		if (false == ret.empty())
		{
			return IsResultOK(ret.value().asString());
		}
		return false;
	}

	// HMGET key field [field ...]
	// Time complexity: O(N)
	//  N : 요청 한 필드 수
	std::map<std::string, Variant> HMGet(int db_type, const std::string& key, const std::list<std::string>& fields)
	{
		std::string searchFields;
		for (const auto& field : fields)
		{
			searchFields += Format(" '", CheckString(field), "'");
		}

		auto ret = Execute(db_type, "HMGET '", CheckString(key), "'", searchFields);

		std::map<std::string, Variant> result;

		int i = 0;
		for (const auto& field : fields)
		{
			if (false == ret[i].empty())
			{
				result[field] = ret[i].asString();
			}

			i++;
		}
		return result;
	}


	// LPUSH key value [value ...]
	// Time complexity: O(1)
	bool LPush(int db_type, const std::string& key, const std::string& value)
	{
		auto ret = Execute(db_type, "LPUSH '", CheckString(key), "' '", CheckString(value), "'");

		if (false == ret.empty())
		{
			return ret.value().asBool();
		}
		return false;
	}

	// LRANGE key start stop
	// Time complexity: O(S+N)
	//  S : head 부터 start 까지의 거리
	//  N : 검색된 객체 수
	std::list<std::string> LRange(int db_type, const std::string& key, int start, int end)
	{
		std::list<std::string> result;
		auto ret = Execute(db_type, "LRANGE '", CheckString(key), "' ", start, " ", end);
		if(0 == ret.size())
		{
			return result;
		}

		for (const auto& row : ret)
		{
			result.push_back(row.asString());
		}
		return result;
	}

	// LREM key count value
	// Time complexity: O(N)
	//  N : 삭제된 객체 수
	int64_t LRem(int db_type, const std::string& key, const std::string& value, int count)
	{
		auto ret = Execute(db_type, "LREM '", CheckString(key), "' ", count, " '", CheckString(value), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// SADD key member [member ...]
	// Time complexity: O(N)
	//  N : 추가 할 멤버 수
	bool SAdd(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "SADD '", CheckString(key), "' '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asBool();
		}
		return false;
	}

	// SCARD key
	// Time complexity: O(1)
	int64_t SCard(int db_type, const std::string& key)
	{
		auto ret = Execute(db_type, "SCARD '", CheckString(key), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// SREM key member [member ...]
	// Time complexity: O(N)
	//  N : 삭제 할 멤버 수
	int64_t SRem(int db_type, const std::string& key, const std::string& member)
	{
		auto ret = Execute(db_type, "SREM '", CheckString(key), "' '", CheckString(member), "'");
		if (false == ret.empty())
		{
			return ret.value().asInt64();
		}
		return 0;
	}

	// SMEMBERS key
	// Time complexity: O(N)
	//  N : 전체 멤버 수
	std::list<Variant> SMembers(int db_type, const std::string& key)
	{
		std::list<Variant> result;
		auto ret = Execute(db_type, "SMEMBERS '", CheckString(key), "'");
		if (0 == ret.size())
		{
			return result;
		}

		for (const auto& row : ret)
		{
			result.push_back(row.asString());
		}
		return result;
	}

	// SRANDMEMBER key [count]
	// Time complexity: O(N)
	//  N : 검색 할 수
	std::list<Variant> SRandMembers(int db_type, const std::string& key, int64_t limitCount)
	{
		std::list<Variant> result;
		auto ret = Execute(db_type, "SRANDMEMBER '", CheckString(key), "' ", limitCount);
		if (0 == ret.size())
		{
			return result;
		}

		for (const auto& row : ret)
		{
			result.push_back(row.asString());
		}
		return result;
	}

} } }
