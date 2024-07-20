#ifndef _MONSTER_ACTION_H_
#define _MONSTER_ACTION_H_

#include "BehaviourTree.h"

#include "../AStarPathFinder.h"
#include "../Dungeon.h"
#include <idl/MessageCommon.h>


namespace Component { 

namespace Unit {
	class Data;
}

namespace Monster { namespace Action {

	class SearchTarget : public BehaviourTree<std::shared_ptr<Unit::Data>>::Action
	{
	public:
		bool Run(std::shared_ptr<Unit::Data> self) override;
	};
	
	class FindPathToTarget : public BehaviourTree<std::shared_ptr<Unit::Data>>::Action
	{
	public :
		bool Run(std::shared_ptr<Unit::Data> self) override;
	};	
	
	class MoveToTarget : public BehaviourTree<std::shared_ptr<Unit::Data>>::Action
	{
	public :
		bool Run(std::shared_ptr<Unit::Data> self) override;
	};
}}
}

#endif // !_UNIT_BEHAVIOUR_H_

