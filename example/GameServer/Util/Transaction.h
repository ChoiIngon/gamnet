#pragma once

#include <list>
#include <memory>
#include <Gamnet/Database/Database.h>

class Transaction
{
public :
    typedef Gamnet::Database::MySQL::ResultSet  ResultSet;
    typedef Gamnet::Database::MySQL::Connection Connection;

    class Statement
    {
    public :
        virtual void Commit(const std::shared_ptr<Connection>& db) = 0;
        virtual void Rollback() = 0;
        virtual void Sync() = 0;
    };

    Transaction(int shard_index);
    ~Transaction() {}

    bool operator ()(const std::shared_ptr<Statement>& statement);

    void Commit();
    void Rollback();

private :
    std::list<std::shared_ptr<Statement>> statements;
    int shard_index;
};
