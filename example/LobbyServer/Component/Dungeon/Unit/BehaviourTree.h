#ifndef BEHAVIOURTREE_H_
#define BEHAVIOURTREE_H_

/**
	https ://engineering.linecorp.com/ko/blog/behavior-tree/
*/

#include <vector>
#include <algorithm>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <Gamnet/Library/Component.h>
#include <Gamnet/Library/Json/json.h>

template <class T>
class BehaviourTree 
{
public:
	class Node
	{
	public:
		virtual ~Node() {}

		void AddChild(const std::shared_ptr<Node>& child)
		{
			children.push_back(child);
		}
		const std::shared_ptr<Node> GetChild(unsigned int index = 0) const
		{
			if (index >= children.size())
			{
				return nullptr;
			}
			return children[index];
		}
		const std::vector<std::shared_ptr<Node>>& GetChildren() const
		{
			return children;
		}
		//std::string name;
		virtual bool Run(T param) = 0;
	private:
		std::vector<std::shared_ptr<Node>> children;
	};

	class Selector : public Node 
	{
	public:
		virtual bool Run(T param) override;
	};

	class RandomSelector : public Node 
	{
	public:
		virtual bool Run(T param) override;
	};

	class Sequence : public Node 
	{
	public:
		virtual bool Run(T param) override;
	};

	/**
		Inverts the result of the child. 
		A child fails and it will return success to its parent, or a child succeeds and it will return failure to the parent.
	*/
	class Inverter : public Node 
	{  
	private:
		virtual bool Run(T param) override;
	};

	/**
		A succeeder will always return success, irrespective of what the child node actually returned. 
		These are useful in cases where you want to process a branch of a tree where a failure is expected or anticipated, 
		but you don¡¯t want to abandon processing of a sequence that branch sits on.
	*/
	class Succeeder : public Node 
	{  
	private:
		virtual bool Run(T param) override;
	};

	/**
		The opposite of a Succeeder, always returning fail.  
		Note that this can be achieved also by using an Inverter and setting its child to a Succeeder.
	*/
	class Failer : public Node 
	{  
	private:
		virtual bool Run(T param) override;
	};

	/**
		A repeater will reprocess its child node each time its child returns a result. 
		These are often used at the very base of the tree, to make the tree to run continuously. 
		Repeaters may optionally run their children a set number of times before returning to their parent.
	*/
	class Repeater : public Node 
	{  
	public :
		Repeater(int num = 0) : repeat_count(num) {}  // By default, never terminate.
		virtual bool Run(T param) override;
	private :
		int repeat_count;
	};

	/**
		Like a repeater, these decorators will continue to reprocess their child. 
		That is until the child finally returns a failure, at which point the repeater will return success to its parent.
	*/
	class RepeatUntilFail : public Node 
	{ 
	private:
		virtual bool Run(T param) override;
	};

	class Action : public Node
	{
	public :
		virtual void Init(const Json::Value& param) {}
	};

	class Meta
	{
	public:
		std::shared_ptr<BehaviourTree::Node> ReadXml(const std::string& path);

		template<class ACTION_T>
		void BindAction(const std::string& name)
		{
			action_creators[name] = []()
			{
				return std::make_shared<ACTION_T>();
			};
		}
	private:
		void LoadNode(std::shared_ptr<BehaviourTree::Node> parent, boost::property_tree::ptree& ptree);
		Json::Value LoadParam(boost::property_tree::ptree& ptree);
	private:
		std::map<std::string, std::function<std::shared_ptr<BehaviourTree::Action>()>> action_creators;
	};

	BehaviourTree();
	
	void Run(T param);

	std::shared_ptr<Node> root;
};

//#include "BehaviourTree.h"
#include <boost/property_tree/xml_parser.hpp>
#include <Gamnet/Library/Exception.h>
#include <idl/MessageCommon.h>

template <class T>
void BehaviourTree<T>::Meta::LoadNode(std::shared_ptr<BehaviourTree<T>::Node> parent, boost::property_tree::ptree& ptree)
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
			const std::string& className = params["class"].asString();
			auto itr = action_creators.find(className);
			if (action_creators.end() == itr)
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
			if (true == params["repeat_count"].isNull())
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

template <class T>
Json::Value BehaviourTree<T>::Meta::LoadParam(boost::property_tree::ptree& ptree)
{
	Json::Value param;
	for (auto& child : ptree.get_child("<xmlattr>"))
	{
		param[child.first] = child.second.get_value<std::string>();
	}
	return param;
}

template <class T>
bool BehaviourTree<T>::Selector::Run(T param)
{
	for (const std::shared_ptr<Node>& child : BehaviourTree<T>::Node::GetChildren())
	{
		if (true == child->Run(param)) {
			return true;
		}
	}
	return false;
}

template <class T>
bool BehaviourTree<T>::RandomSelector::Run(T param)
{
	std::vector<std::shared_ptr<Node>> temp = BehaviourTree<T>::Node::GetChildren();
	// The order is shuffled
	std::random_shuffle(temp.begin(), temp.end());
	for (const std::shared_ptr<Node>& child : temp)
	{
		if (true == child->Run(param))
		{
			return true;
		}
	}
	return false;
}

template <class T>
bool BehaviourTree<T>::Sequence::Run(T param)
{
	for (const std::shared_ptr<Node>& child : BehaviourTree<T>::Node::GetChildren())
	{
		if (false == child->Run(param))
		{
			return false;
		}
	}
	return true;
}

template <class T>
bool BehaviourTree<T>::Inverter::Run(T param)
{
	return !BehaviourTree<T>::Node::GetChild()->Run(param);
}

template <class T>
bool BehaviourTree<T>::Succeeder::Run(T param)
{
	BehaviourTree<T>::Node::GetChild()->Run(param);
	return true;
}

template <class T>
bool BehaviourTree<T>::Failer::Run(T param)
{
	BehaviourTree<T>::Node::GetChild()->Run(param);
	return false;
}

template <class T>
bool BehaviourTree<T>::Repeater::Run(T param)
{
	for (int i = 0; i < repeat_count; i++)
	{
		if (false == BehaviourTree<T>::Node::GetChild()->Run(param))
		{
			return false;
		}
	}
	return true;
}

template <class T>
bool BehaviourTree<T>::RepeatUntilFail::Run(T param)
{
	while (BehaviourTree<T>::Node::GetChild()->Run(param)) {}
	return true;
}

template <class T>
class RootNode : public BehaviourTree<T>::Node
{
public:
	virtual bool Run(T param) override
	{
		return BehaviourTree<T>::Node::GetChild()->Run(param);
	}
};

template <class T>
BehaviourTree<T>::BehaviourTree()
	: root(nullptr)
{
}

template <class T>
void BehaviourTree<T>::Run(T param)
{
	if (nullptr == root)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	root->Run(param);
}

template <class T>
std::shared_ptr<typename BehaviourTree<T>::Node> BehaviourTree<T>::Meta::ReadXml(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Gamnet::Exception(Gamnet::ErrorCode::FileNotFound, e.what());
	}

	std::shared_ptr<BehaviourTree::Node> root = std::make_shared<RootNode<T>>();
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

#endif /* BEHAVIOURTREE_H_ */