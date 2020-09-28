#include "UnitBehaviour.h"
#include <Gamnet/Library/Exception.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

Executor::Executor(const std::shared_ptr<UnitBehaviour>& tree)
	: tree(tree)
{
}

void LoadParams(std::shared_ptr<Config::Node> parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if("class" == child.first)
		{
			continue;
		}
		parent->params[child.first] = child.second.get_value<std::string>();
	}
}

void LoadBehaviour(std::shared_ptr<Config::Node> parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if ("<xmlattr>" == child.first)
		{
			LoadParams(parent, child.second);
			return;
		}

		std::shared_ptr<Config::Node> node = nullptr;
		if ("Sequence" == child.first)
		{
			node = std::make_shared<Config::Node>();
			node->name = "Sequence";
			node->type = "Sequence";
			node->create = []() {
				return std::make_shared<BehaviourTree::Sequence>();
			};
		}
		else if("Executor" == child.first)
		{
			node = std::make_shared<Config::Node>();
			node->name = "Executor";
			node->type = child.second.get_child("<xmlattr>.class").get_value<std::string>();
		}
		else if("RandomSelector" == child.first)
		{
			node = std::make_shared<Config::Node>();
			node->name = "RandomSelector";
			node->type = "RandomSelector";
			node->create = []() {
				return std::make_shared<BehaviourTree::RandomSelector>();
			};
		}
		if(nullptr == node)
		{
			return;
		}
		parent->children.push_back(node);
	/*
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
	*/
		LoadBehaviour(node, child.second);
	}
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

	root = std::make_shared<Node>();
	try {
		for (auto& child : ptree_.get_child(""))
		{
			LoadBehaviour(root, child.second);
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		throw GAMNET_EXCEPTION(Gamnet::ErrorCode::SystemInitializeError, e.what());
	}
}
