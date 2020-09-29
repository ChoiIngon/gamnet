#include "MonsterBehaviour.h"
#include <Gamnet/Library/Exception.h>
#include <idl/MessageCommon.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>

bool RiseFromChair::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::cout << "rise from chair" << std::endl;
	return true;
}

bool MoveToVendingMachine::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::cout << "move to vending machine" << std::endl;
	return true;
}

bool BuyTea::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::cout << "buy a cup of tea" << std::endl;
	return true;
}

bool BuyCoffee::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::cout << "buy a cup of coffee" << std::endl;
	return true;
}

bool ReturnToChair::Run(const std::shared_ptr<Gamnet::Component>& component)
{
	std::cout << "return to chair" << std::endl;
	return true;
}
