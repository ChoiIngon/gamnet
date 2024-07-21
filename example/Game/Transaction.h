#pragma once

#include <string>
#include <list>
#include <memory>
#include "Session.h"
#include <Gamnet/Database/MySQL/MySQL.h>

class Transaction
{
public:
	typedef Gamnet::Database::MySQL::ResultSet ResultSet;
	struct Query
	{
		virtual std::string Commit() = 0;
		virtual void Rollback() {};
	};

public:
	Transaction(int index);
	virtual ~Transaction();

	void Execute(const std::shared_ptr<Query>& query);

	ResultSet Commit();
	void Rollback();

private:
	void Clear();

private:
	int db_index;
	std::list<std::shared_ptr<Query>> queries;
};