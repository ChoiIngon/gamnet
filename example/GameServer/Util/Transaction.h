#pragma once

#include <list>
#include <memory>
#include <Gamnet/Database/Database.h>
#include <Gamnet/Library/Delegate.h>
#include <Gamnet/Library/Return.h>

class UserSession;
class Transaction
{
public :
    typedef Gamnet::Database::MySQL::ResultSet  ResultSet;
    typedef Gamnet::Database::MySQL::Connection Connection;

    struct Statement
    {
        typedef Gamnet::Delegate<void(const std::shared_ptr<UserSession>&, const std::shared_ptr<Connection>&)> CommitDelegate;
        typedef Gamnet::Delegate<void(const std::shared_ptr<UserSession>&)> RollbackDelegate;
        typedef Gamnet::Delegate<void(const std::shared_ptr<UserSession>&)> SyncDelegate;

        CommitDelegate Commit;
        RollbackDelegate Rollback;
        SyncDelegate Sync;
    };

    Transaction(const std::shared_ptr<UserSession>& session);
    ~Transaction() {}

    bool operator ()(const std::shared_ptr<Statement>& statement);
    bool operator ()(const Gamnet::Return<std::shared_ptr<Statement>>& result);
    void Commit();
    void Rollback();

private :
    std::list<std::shared_ptr<Statement>> statements;
    std::shared_ptr<UserSession> session;
};
