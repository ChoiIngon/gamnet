#ifndef BEHAVIOURTREE_H_
#define BEHAVIOURTREE_H_

/**
	https ://engineering.linecorp.com/ko/blog/behavior-tree/
*/

#include <vector>
#include <algorithm>
#include <memory>
#include <string>

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
		virtual bool Run() = 0;
	private :
		std::vector<std::shared_ptr<Node>> children;
	};

	class Selector : public Node 
	{
	public:
		virtual bool Run() override;
	};

	class RandomSelector : public Node 
	{
	public:
		virtual bool Run() override;
	};

	class Sequence : public Node 
	{
	public:
		virtual bool Run() override;
	};

	/**
		Inverts the result of the child. 
		A child fails and it will return success to its parent, or a child succeeds and it will return failure to the parent.
	*/
	class Inverter : public Node 
	{  
	private:
		virtual bool Run() override;
	};

	/**
		A succeeder will always return success, irrespective of what the child node actually returned. 
		These are useful in cases where you want to process a branch of a tree where a failure is expected or anticipated, 
		but you don¡¯t want to abandon processing of a sequence that branch sits on.
	*/
	class Succeeder : public Node 
	{  
	private:
		virtual bool Run() override;
	};

	/**
		The opposite of a Succeeder, always returning fail.  
		Note that this can be achieved also by using an Inverter and setting its child to a Succeeder.
	*/
	class Failer : public Node 
	{  
	private:
		virtual bool Run() override;
	};

	/**
		A repeater will reprocess its child node each time its child returns a result. 
		These are often used at the very base of the tree, to make the tree to run continuously. 
		Repeaters may optionally run their children a set number of times before returning to their parent.
	*/
	class Repeater : public Node 
	{  
	private:
		int repeat_count;
		static const int NOT_FOUND = -1;
		Repeater(int num = NOT_FOUND) : repeat_count(num) {}  // By default, never terminate.
		virtual bool Run() override;
	};

	/**
		Like a repeater, these decorators will continue to reprocess their child. 
		That is until the child finally returns a failure, at which point the repeater will return success to its parent.
	*/
	class RepeatUntilFail : public Node 
	{ 
	private:
		virtual bool Run() override;
	};

	BehaviourTree();
	void Run();

	const std::shared_ptr<Node> root;
};
#endif /* BEHAVIOURTREE_H_ */