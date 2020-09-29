#ifndef BEHAVIOURTREE_H_
#define BEHAVIOURTREE_H_

/**
	https ://engineering.linecorp.com/ko/blog/behavior-tree/
*/

#include <vector>
#include <algorithm>
#include <memory>
#include <Gamnet/Library/Component.h>
#include <boost/property_tree/ptree.hpp>
#include <Gamnet/Library/Json/json.h>

class BehaviourTree 
{
public:
	class Node
	{
	public:
		Node();
		virtual ~Node() {}

		void AddChild(const std::shared_ptr<Node>& child);
		const std::shared_ptr<Node> GetChild(unsigned int index = 0) const;
		const std::vector<std::shared_ptr<Node>>& GetChildren() const;

		//std::string name;
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) = 0;
	private:
		std::vector<std::shared_ptr<Node>> children;
	};

	class Meta
	{
	public:
		struct Node
		{
			std::map<std::string, std::string> params;
			std::vector<std::shared_ptr<Node>> children;
			std::function<std::shared_ptr<BehaviourTree::Node>()> create;
		};

		void ReadXml(const std::string& path);
		std::shared_ptr<BehaviourTree> Create();

		template<class T>
		void BindExcutorCreator(const std::string& name)
		{
			creators[name] = []()
			{
				return std::make_shared<T>();
			};
		}
	private:
		void Create(std::shared_ptr<BehaviourTree::Node> behaviourNode, std::shared_ptr<Node> configNode);
		void LoadNode(std::shared_ptr<Node> parent, boost::property_tree::ptree& ptree);
		void LoadParam(std::shared_ptr<Node> parent, boost::property_tree::ptree& ptree);
	private:
		std::shared_ptr<Node> root;
		std::map<std::string, std::function<std::shared_ptr<BehaviourTree::Node>()>> creators;
	};

	class Selector : public Node 
	{
	public:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	class RandomSelector : public Node 
	{
	public:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	class Sequence : public Node 
	{
	public:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	/**
		Inverts the result of the child. 
		A child fails and it will return success to its parent, or a child succeeds and it will return failure to the parent.
	*/
	class Inverter : public Node 
	{  
	private:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	/**
		A succeeder will always return success, irrespective of what the child node actually returned. 
		These are useful in cases where you want to process a branch of a tree where a failure is expected or anticipated, 
		but you don¡¯t want to abandon processing of a sequence that branch sits on.
	*/
	class Succeeder : public Node 
	{  
	private:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	/**
		The opposite of a Succeeder, always returning fail.  
		Note that this can be achieved also by using an Inverter and setting its child to a Succeeder.
	*/
	class Failer : public Node 
	{  
	private:
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
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
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
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
		virtual bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
	};

	class Action : public Node
	{
	public :
		Action(const Json::Value& params);
		Json::Value const params;
	};

	BehaviourTree();
	void Run(const std::shared_ptr<Gamnet::Component>& component);

	const std::shared_ptr<Node> root;
};
#endif /* BEHAVIOURTREE_H_ */