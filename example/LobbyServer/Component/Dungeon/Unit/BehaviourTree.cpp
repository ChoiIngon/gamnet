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

void BehaviourTree::Meta::LoadNode(std::shared_ptr<BehaviourTree::Node> parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if ("<xmlattr>" == child.first)
		{
			continue;
		}

		std::shared_ptr<BehaviourTree::Node> node = nullptr;
		if ("Sequence" == child.first)
		{
			node = std::make_shared<BehaviourTree::Sequence>();
		}
		else if ("Action" == child.first)
		{
			Json::Value params = LoadParam(child.second);
			if (true == params["class"].isNull())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "Action@class is null");
			}
			std::string& className = params["class"].asString();
			auto itr = action_creators.find(className);
			if(action_creators.end() == itr)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "behaviour tree executor is not defined(name:", className, ")");
			}
			std::shared_ptr<BehaviourTree::Action> action = itr->second();
			action->Init(params);
			node = action;
		}
		else if ("Selector" == child.first)
		{
			node = std::make_shared<BehaviourTree::Selector>();
		}
		else if ("RandomSelector" == child.first)
		{
			node = std::make_shared<BehaviourTree::RandomSelector>();
		}
		else if ("Inverter" == child.first)
		{
			node = std::make_shared<BehaviourTree::Inverter>();
		}
		else if ("Succeeder" == child.first)
		{
			node = std::make_shared<BehaviourTree::Succeeder>();
		}
		else if ("Failer" == child.first)
		{
			node = std::make_shared<BehaviourTree::Failer>();
		}
		else if ("Repeater" == child.first)
		{
			Json::Value params = LoadParam(child.second);
			if(true == params["repeat_count"].isNull())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "Repeater@repeat_count is null");
			}
			int repeatCount = params["repeat_count"].asInt();
			
			node = std::make_shared<BehaviourTree::Repeater>(repeatCount);
		}
		else if ("RepeatUntilFail" == child.first)
		{
			node = std::make_shared<BehaviourTree::RepeatUntilFail>();
		}
		else
		{
			return;
		}
		parent->AddChild(node);
		LoadNode(node, child.second);
	}
}
Json::Value BehaviourTree::Meta::LoadParam(boost::property_tree::ptree& ptree)
{
	Json::Value param;
	for (auto& child : ptree.get_child("<xmlattr>"))
	{
		param[child.first] = child.second.get_value<std::string>();
	}
	return param;
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
	for (int i = 0; i < repeat_count; i++)
	{
		if(false == GetChild()->Run(component))
		{
			return false;
		}
	}
	return true;
}

bool BehaviourTree::RepeatUntilFail::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	while (GetChild()->Run(component)) {}
	return true;
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
	: root(nullptr)
{
}

void BehaviourTree::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	if(nullptr == root)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	root->Run(component);
}

std::shared_ptr<BehaviourTree::Node> BehaviourTree::Meta::ReadXml(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Gamnet::Exception(Gamnet::ErrorCode::FileNotFound, e.what());
	}

	std::shared_ptr<BehaviourTree::Node> root = std::make_shared<RootNode>();
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
	return root;
}
