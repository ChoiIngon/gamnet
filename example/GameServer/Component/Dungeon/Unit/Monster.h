#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "../../../Util/MetaData.h"
#include "../Vector2.h"
#include "MonsterAction.h"

namespace Component { namespace Unit {
	class Data;
}}

namespace Component { namespace Dungeon {
	class Data;
}}

namespace Component { namespace Monster {
	class Meta : public MetaData
	{
	public :
		Meta();

		std::string id;
		uint32_t	index;
		std::string name;
		std::shared_ptr<BehaviourTree<std::shared_ptr<Unit::Data>>> behaviour;

		bool IsVisible(std::shared_ptr<Unit::Data>& self, const Vector2Int& target);
		void Update(std::shared_ptr<Unit::Data> data);
	private :
		void OnBehaviourPath(std::shared_ptr<BehaviourTree<std::shared_ptr<Unit::Data>>>& behaviour, const std::string& value);
	};

	class Data
	{
	public :
		Data(std::shared_ptr<Meta> meta);
		virtual ~Data();

		const std::shared_ptr<Meta> meta;
		std::shared_ptr<Unit::Data> target;
		std::list<Vector2Int> path;
	};
}}
#endif
