#pragma once
#ifndef _UNIT_BEHAVIOUR_H_
#define _UNIT_BEHAVIOUR_H_

#include "BehaviourTree.h"
#include "../Unit.h"
#include "Player.h"

struct MonsterBehaviour
{
	MonsterBehaviour()
		: target(nullptr)
	{
	}
	std::shared_ptr<Unit> target;
};

class SearchTarget : public BehaviourTree<Unit>::Action
{
public:
	bool Run(Unit& unit) override;
};

class MoveToTarget : public BehaviourTree<Unit>::Action
{
public :
	bool Run(Unit& unit) override;
};

class RiseFromChair : public BehaviourTree<Unit>::Action
{
public:
	bool Run(Unit& unit) override;
};

class MoveToVendingMachine : public BehaviourTree<Unit>::Action
{
public:
	void Init(const Json::Value& param);
	bool Run(Unit& unit) override;
private :
	int money;
};

class BuyTea : public BehaviourTree<Unit>::Action
{
public:
	bool Run(Unit& unit) override;
};

class BuyCoffee : public BehaviourTree<Unit>::Action
{
public:
	bool Run(Unit& unit) override;
};

class ReturnToChair : public BehaviourTree<Unit>::Action
{
public:
	bool Run(Unit& unit) override;
};

#endif // !_UNIT_BEHAVIOUR_H_

