#ifndef _DUNGEON_H
#define	_DUNGEON_H

#include <Gamnet/Network/Network.h>
#include <Gamnet/Library/Pool.h>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "Rect.h"
#include "TileMap.h"
#include "../../Util/Table.h"
#include <idl/MessageCommon.h>
#include <idl/MessageDungeon.h>

class Player;
class UserSession;

namespace Component { namespace Dungeon {
	class Data;
	struct Meta : public std::enable_shared_from_this<Meta>, public Table::MetaData<Meta>
	{
		struct Room : public Table::MetaData<Room>
		{
			Room();
			int Count;
			int MinWidth;
			int MaxWidth;
			int MinHeight;
			int MaxHeight;
		};

		struct Monster : public Table::MetaData<Monster>
		{
			Monster();
			std::string Code;
			int Level;
			int Count;
		};

		Meta();

		std::string Code;
		int			Index;
		int			Level;
		Room		Room;
		std::vector<std::shared_ptr<Monster>> Monster;

		std::shared_ptr<Data> CreateInstance();

		virtual void OnLoad() override;
	};

	class Data : public TileMap
	{
		struct Block
		{
			enum class Type
			{
				Invalid,
				Corridor,
				Room
			};

			Block(int id);

			const int id;
			Type type;
			RectInt rect;
		};

		struct Edge
		{
			std::shared_ptr<Block> block;
			std::shared_ptr<Edge> parent;
			int distance;
			bool visit;
		};
	public :
		Data(const Meta& meta);
		void Init();

		const Meta& meta;
		uint64_t seq;
		std::shared_ptr<Gamnet::Network::Session::Strand> strand;
		std::shared_ptr<Block> start;
		std::shared_ptr<Block> end;
		std::map<uint64_t, std::shared_ptr<UserSession>> sessions;
		std::map<uint64_t, std::shared_ptr<Unit::Data>> players;
		std::map<uint64_t, std::shared_ptr<Unit::Data>> monster;
		size_t GetPlayerCount() const;

		size_t Enter(std::shared_ptr<UserSession> session);
		void Leave(std::shared_ptr<UserSession> session);
		
		template <class MSG>
		void SendMsg(const MSG& msg, std::shared_ptr<UserSession> except = nullptr)
		{
			for (auto itr : sessions)
			{
				std::shared_ptr<UserSession> session = itr.second;
				if(except == session)
				{
					continue;
				}

				Gamnet::Network::Tcp::SendMsg(session, msg, true);
			}
		}
	private :
		std::list<std::shared_ptr<Block>> FindNeighborBlocks(std::shared_ptr<Block> block);
		std::list<Vector2Int> BuildPath();
		void MoveToEmptySpace(std::shared_ptr<Block> block);
		bool OverlapWithExistBlocks(std::shared_ptr<Block> block);
	private :
		static constexpr int ROOM_COUNT_MULTIPLE = 5;
		static const Vector2Int EIGHT_DIRECTION_OFFSET[8];
		std::map<int, std::shared_ptr<Block>> blocks;

		Gamnet::Pool<Gamnet::Time::Timer> timer_pool;

	};

	class Manager
	{
		class WaitQueue
		{
		public :
			static constexpr size_t MAX_PLAER = 2;
		private :
			std::array<std::list<std::shared_ptr<Data>>, MAX_PLAER + 1> queue_by_player_count;

			std::mutex lock;
		public :
			std::shared_ptr<Data> Pop();
			void Push(std::shared_ptr<Data> data);
		};
	public :
		void Init();

		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
		std::shared_ptr<Data> Enter(uint32_t index, std::shared_ptr<UserSession> session);
	private :
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
		std::map<uint32_t, std::shared_ptr<WaitQueue>> wait_queues;
	};

	std::shared_ptr<Data> Enter(std::shared_ptr<UserSession> session, uint32_t index);
};

}

#endif
