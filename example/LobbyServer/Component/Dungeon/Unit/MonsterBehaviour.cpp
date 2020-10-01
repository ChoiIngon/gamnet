#include "MonsterBehaviour.h"
#include <Gamnet/Library/Exception.h>
#include <idl/MessageCommon.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>

#include "../Dungeon.h"
bool SearchTarget::Run(Unit& unit)
{
	std::shared_ptr<MonsterBehaviour> behaviour = unit.attributes->GetComponent<MonsterBehaviour>();
	if(nullptr == behaviour)
	{
		behaviour = unit.attributes->AddComponent<MonsterBehaviour>();
	}

	if(nullptr != behaviour->target)
	{
		return true;
	}

	std::shared_ptr<Component::Dungeon> dungeon = unit.attributes->GetComponent<Component::Dungeon>();
	float distance = Vector2Int::Distance(dungeon->player->position, unit.position);
	if(distance <= unit.sight)
	{
		behaviour->target = dungeon->player;
		return true;
	}

	return false;
}

bool MoveToTarget::Run(Unit& unit)
{
	std::cout << "rise from chair" << std::endl;
	return true;
}
bool RiseFromChair::Run(Unit& unit)
{
	std::cout << "rise from chair" << std::endl;
	return true;
}

void MoveToVendingMachine::Init(const Json::Value& param)
{
	money = param["money"].asInt();
}

bool MoveToVendingMachine::Run(Unit& unit)
{
	std::cout << "move to vending machine " << money << std::endl;
	return true;
}

bool BuyTea::Run(Unit& unit)
{
	std::cout << "buy a cup of tea" << std::endl;
	return true;
}

bool BuyCoffee::Run(Unit& unit)
{
	std::cout << "buy a cup of coffee" << std::endl;
	return true;
}

bool ReturnToChair::Run(Unit& unit)
{
	std::cout << "return to chair" << std::endl;
	return true;
}
