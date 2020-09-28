#ifndef _UNIT_BEHAVIOUR_H_
#define _UNIT_BEHAVIOUR_H_

#include "Unit/BehaviourTree.h"
#include <map>

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
	struct Node
	{
		std::string name;
		std::string type;
		std::map<std::string, std::string> params;
		std::vector<std::shared_ptr<Node>> children;
		std::function<std::shared_ptr<BehaviourTree::Node>()> create;
	};

	void ReadXml(const std::string& path);
	std::shared_ptr<UnitBehaviour> Create();
	std::shared_ptr<Node> root;
};

#endif // !_UNIT_BEHAVIOUR_H_

