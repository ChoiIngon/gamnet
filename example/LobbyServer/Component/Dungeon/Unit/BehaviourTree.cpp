#include "BehaviourTree.h"
#include <boost/property_tree/xml_parser.hpp>
#include <Gamnet/Library/Exception.h>
#include <idl/MessageCommon.h>

BehaviourTree::Node::Node()
{
}

void BehaviourTree::Node::AddChild(const std::shared_ptr<Node>& child)
{
	children.push_back(child);
}

const std::shared_ptr<BehaviourTree::Node> BehaviourTree::Node::GetChild(unsigned int index) const
{
	if (index >= children.size())
	{
		return nullptr;
	}
	return children[index];
}

const std::vector<std::shared_ptr<BehaviourTree::Node>>& BehaviourTree::Node::GetChildren() const
{
	return children;
}

void BehaviourTree::Meta::ReadXml(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Gamnet::Exception(Gamnet::ErrorCode::FileNotFound, e.what());
	}

	root = std::make_shared<Node>();
	try {
		for (auto& child : ptree_.get_child(""))
		{
			LoadNode(root, child.second);
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		throw GAMNET_EXCEPTION(Gamnet::ErrorCode::SystemInitializeError, e.what());
	}
}

void BehaviourTree::Meta::LoadNode(std::shared_ptr<BehaviourTree::Meta::Node> parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if ("<xmlattr>" == child.first)
		{
			LoadParam(parent, child.second);
			continue;
		}

		std::shared_ptr<BehaviourTree::Meta::Node> node = std::make_shared<BehaviourTree::Meta::Node>();
		if ("Sequence" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::Sequence>();
			};
		}
		else if ("Executor" == child.first)
		{
			std::string& className = child.second.get_child("<xmlattr>.class").get_value<std::string>();
			node->create = [this, className]() {
				auto itr = this->creators.find(className);
				if(this->creators.end() == itr)
				{
					throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "behaviour tree executor is not defined(name:", className, ")");
				}
				return this->creators[className]();
			};
		}
		else if ("Selector" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::Selector>();
			};
		}
		else if ("RandomSelector" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::RandomSelector>();
			};
		}
		else if ("Inverter" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::Inverter>();
			};
		}
		else if ("Succeeder" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::Succeeder>();
			};
		}
		else if ("Failer" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::Failer>();
			};
		}
		else if ("Repeater" == child.first)
		{
			node->create = [node]() {
				int repeatCount = 0;
				auto itr = node->params.find("repeat_count");
				if (node->params.end() == itr)
				{
					throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
				}

				repeatCount = std::stoi(itr->second);

				return std::make_shared<BehaviourTree::Repeater>(repeatCount);
			};
		}
		else if ("RepeatUntilFail" == child.first)
		{
			node->create = []() {
				return std::make_shared<BehaviourTree::RepeatUntilFail>();
			};
		}
		else
		{
			return;
		}
		parent->children.push_back(node);
		LoadNode(node, child.second);
	}
}
void BehaviourTree::Meta::LoadParam(std::shared_ptr<BehaviourTree::Meta::Node> parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if ("class" == child.first)
		{
			continue;
		}
		parent->params[child.first] = child.second.get_value<std::string>();
	}
}

std::shared_ptr<BehaviourTree> BehaviourTree::Meta::Create()
{
	std::shared_ptr<BehaviourTree> behaviour = std::make_shared<BehaviourTree>();
	Create(behaviour->root, root);
	return behaviour;
}

void BehaviourTree::Meta::Create(std::shared_ptr<BehaviourTree::Node> behaviourNode, std::shared_ptr<BehaviourTree::Meta::Node> configNode)
{
	for (std::shared_ptr<BehaviourTree::Meta::Node>& configChildNode : configNode->children)
	{
		std::shared_ptr<BehaviourTree::Node> behaviourChildNode = configChildNode->create();
		behaviourNode->AddChild(behaviourChildNode);
		Create(behaviourChildNode, configChildNode);
	}
}

bool BehaviourTree::Selector::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	for (const std::shared_ptr<Node>& child : GetChildren()) 
	{
		if (true == child->Run(component)) {
			return true;
		}
	}
	return false;
}

bool BehaviourTree::RandomSelector::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::vector<std::shared_ptr<Node>> temp = GetChildren();
	// The order is shuffled
	std::random_shuffle(temp.begin(), temp.end());
	for (const std::shared_ptr<Node>& child : temp) 
	{  
		if (true == child->Run(component))
		{
			return true;
		}
	}
	return false;
}

bool BehaviourTree::Sequence::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	for (const std::shared_ptr<Node>& child : GetChildren()) 
	{
		if (false == child->Run(component)) 
		{
			return false;
		}
	}
	return true;
}

bool BehaviourTree::Inverter::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	return !GetChild()->Run(component);
}

bool BehaviourTree::Succeeder::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	GetChild()->Run(component);
	return true;
}

bool BehaviourTree::Failer::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	GetChild()->Run(component);
	return false;
}

bool BehaviourTree::Repeater::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	for (int i = 0; i < repeat_count - 1; i++)
	{
		GetChild()->Run(component);
	}
	return GetChild()->Run(component);
}

bool BehaviourTree::RepeatUntilFail::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	while (GetChild()->Run(component)) {}
	return true;
}

BehaviourTree::Action::Action(const Json::Value& params)
	: params(params)
{
}

class RootNode : public BehaviourTree::Node
{
public:
	virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override
	{
		return GetChild()->Run(component);
	}
};

BehaviourTree::BehaviourTree()
	: root(std::make_shared<RootNode>())
{
}

void BehaviourTree::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	root->Run(component);
}
