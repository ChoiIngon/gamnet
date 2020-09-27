#include "BehaviourTree.h"

BehaviourTree::Node::Node()
{
}

void BehaviourTree::Node::AddChild(const std::shared_ptr<Node>& child)
{
	children.push_back(child);
}

const std::shared_ptr<BehaviourTree::Node> BehaviourTree::Node::GetChild(unsigned int index) const
{
	if (index > children.size())
	{
		return NULL;
	}
	return children[index];
}

const std::vector<std::shared_ptr<BehaviourTree::Node>>& BehaviourTree::Node::GetChildren() const
{
	return children;
}

bool BehaviourTree::Selector::Run()
{
	for (const std::shared_ptr<Node>& child : GetChildren()) {
		if (true == child->Run()) {
			return true;
		}
	}
	return false;
}

bool BehaviourTree::RandomSelector::Run()
{
	std::vector<std::shared_ptr<Node>> temp = GetChildren();
	std::random_shuffle(temp.begin(), temp.end());
	for (const std::shared_ptr<Node>& child : temp) {  // The order is shuffled
		if (true == child->Run())
		{
			return true;
		}
	}
	return false;
}

bool BehaviourTree::Sequence::Run()
{
	for (const std::shared_ptr<Node>& child : GetChildren()) 
	{
		if (false == child->Run()) 
		{
			return false;
		}
	}
	return true;
}

bool BehaviourTree::Inverter::Run()
{
	return !GetChild()->Run();
}

bool BehaviourTree::Succeeder::Run()
{
	GetChild()->Run();
	return true;
}

bool BehaviourTree::Failer::Run()
{
	GetChild()->Run();
	return false;
}

bool BehaviourTree::Repeater::Run()
{
	for (int i = 0; i < repeat_count - 1; i++)
	{
		GetChild()->Run();
	}
	return GetChild()->Run();
}

bool BehaviourTree::RepeatUntilFail::Run()
{
	while (GetChild()->Run()) {}
	return true;
}

class RootNode : public BehaviourTree::Node
{
public:
	virtual bool Run() override
	{
		return GetChild()->Run();
	}
};

BehaviourTree::BehaviourTree()
	: root(std::make_shared<RootNode>())
{
}

void BehaviourTree::Run()
{
	root->Run();
}
