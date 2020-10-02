#ifndef _MONSTER_ACTION_H_
#define _MONSTER_ACTION_H_

#include "BehaviourTree.h"
#include "Player.h"
#include "../Unit.h"
#include "../AStarPathFinder.h"
#include "../Dungeon.h"

#include <Gamnet/Library/Exception.h>
#include <idl/MessageCommon.h>

namespace Component { namespace Monster { namespace Action {

	class SearchTarget : public BehaviourTree<std::shared_ptr<Unit>>::Action
	{
	public:
		bool Run(std::shared_ptr<Unit> self) override;
	};
	
	class FindPathToTarget : public BehaviourTree<std::shared_ptr<Unit>>::Action
	{
	public :
		bool Run(std::shared_ptr<Unit> self) override;
	};	
	
	class MoveToTarget : public BehaviourTree<std::shared_ptr<Unit>>::Action
	{
	public :
		bool Run(std::shared_ptr<Unit> self) override;
	};
}}}

#endif // !_UNIT_BEHAVIOUR_H_

