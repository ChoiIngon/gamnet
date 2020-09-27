#ifndef _UNIT_BEHAVIOUR_H_
#define _UNIT_BEHAVIOUR_H_

#include "Unit/BehaviourTree.h"

class Unit;
namespace Component {
	class Dungeon;
};

class UnitBehaviour : public BehaviourTree
{
public :
	std::shared_ptr<Unit> unit;
	std::shared_ptr<Component::Dungeon> dungeon;
};

class Executor : public BehaviourTree::Node
{
public:
	Executor(const std::shared_ptr<UnitBehaviour>& tree);
protected:
	std::weak_ptr<UnitBehaviour> tree;
};

struct Config
{
	void ReadXml(const std::string& path);
	std::shared_ptr<UnitBehaviour> Create();

	
};

#endif // !_UNIT_BEHAVIOUR_H_

