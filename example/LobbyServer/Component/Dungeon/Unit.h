#ifndef _UNIT_H_
#define _UNIT_H_

#include <Gamnet/Library/Component.h>
#include <Gamnet/Library/Time/Timer.h>
#include "Unit/BehaviourTree.h"
#include "Vector2.h"
#include "AStarPathFinder.h"
#include "../../Util/MetaData.h"
#include <string>
#include "Unit/Monster.h"

namespace Component { namespace Dungeon {
	class Data;
}}

namespace Component { namespace Unit {
	class Meta : public MetaData
	{
	public :
		std::string id;
		uint32_t	index;
		int max_health;
		int attack;
		int defense;
		int speed;
		int sight;
		float critical_chance;
		float critical_rate;

		std::shared_ptr<Component::Monster::Meta> monster;

		Meta();
	};
	
	class Data : public std::enable_shared_from_this<Data>, public Gamnet::Component
	{
	public :
		Data();

	public :
		::Component::Dungeon::Data* dungeon;
		std::shared_ptr<Meta> meta;

		const uint64_t seq;

		int max_health;
		int cur_health;
		int attack;
		int defense;
		int speed;
		int	sight;
		float critical_chance;
		float critical_rate;
		Vector2Int	position;

		int team_index;
		void SetPosition(const Vector2Int& position);

		std::shared_ptr<AStarPathFinder> path_finder;
		std::shared_ptr<Gamnet::Time::Timer> timer;
	};

	class Manager
	{
	public:
		void Init();
		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
		std::shared_ptr<Unit::Data> CreateInstance(const std::string& id);
		std::shared_ptr<Unit::Data> CreateInstance(uint32_t index);
	private:
		void InitMeta(const std::string& path);
	private :
		std::shared_ptr<Unit::Data> CreateInstance(const std::shared_ptr<Meta>& meta);
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};

	std::shared_ptr<Unit::Data> CreatePlayer(std::shared_ptr<UserSession> seesion, const std::string& id);
}}
#endif
