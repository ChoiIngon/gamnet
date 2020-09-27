#include "UnitBehaviour.h"
#include <Gamnet/Library/Exception.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

Executor::Executor(const std::shared_ptr<UnitBehaviour>& tree)
	: tree(tree)
{
}

void LoadBehaviour(boost::property_tree::ptree& ptree)
{
	/*
	for (auto& child : ptree.get_child(""))
	{
		if ("<xmlattr>" == child.first)
		{
			return;
		}

		std::shared_ptr<BehaviourTree::Node> node = nullptr;

		if ("Sequence" == child.first)
		{
			[]() {
				std::make_shared< BehaviourTree::Sequence>();
			};
			node = std::make_shared< BehaviourTree::Sequence>();
		}
		if ("RandomSelector" == child.first)
		{
			node = std::make_shared< BehaviourTree::RandomSelector>();
		}
		else if ("Executor" == child.first)
		{
			const std::string& className = child.second.get_child("<xmlattr>.class").get_value<std::string>();
			node = executor_factory.Create(behaviour, className);
		}

	}
	*/
}


void Config::ReadXml(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Gamnet::Exception(Gamnet::ErrorCode::FileNotFound, e.what());
	}

	//try {
	//	for (auto& child : ptree_.get_child(""))
	//	{
	//		LoadBehaviour(behaviour, behaviour->root, child.second);
	//	}
	//}
	//catch (const boost::property_tree::ptree_bad_path& e)
	//{
	//	throw GAMNET_EXCEPTION(Gamnet::ErrorCode::SystemInitializeError, e.what());
	//}
	//return behaviour;
}
