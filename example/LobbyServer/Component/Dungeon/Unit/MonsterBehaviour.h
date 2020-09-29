#pragma once
#ifndef _UNIT_BEHAVIOUR_H_
#define _UNIT_BEHAVIOUR_H_

#include "BehaviourTree.h"

class RiseFromChair : public BehaviourTree::Action
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class MoveToVendingMachine : public BehaviourTree::Action
{
public:
	void Init(const Json::Value& param);
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
private :
	int money;
};

class BuyTea : public BehaviourTree::Action
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class BuyCoffee : public BehaviourTree::Action
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class ReturnToChair : public BehaviourTree::Action
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

#endif // !_UNIT_BEHAVIOUR_H_

