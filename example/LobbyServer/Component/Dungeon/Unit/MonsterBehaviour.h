#pragma once
#ifndef _UNIT_BEHAVIOUR_H_
#define _UNIT_BEHAVIOUR_H_

#include "BehaviourTree.h"

class RiseFromChair : public BehaviourTree::Node
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class MoveToVendingMachine : public BehaviourTree::Node
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class BuyTea : public BehaviourTree::Node
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class BuyCoffee : public BehaviourTree::Node
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

class ReturnToChair : public BehaviourTree::Node
{
public:
	bool Run(const std::shared_ptr<Gamnet::Component>& component) override;
};

#endif // !_UNIT_BEHAVIOUR_H_

