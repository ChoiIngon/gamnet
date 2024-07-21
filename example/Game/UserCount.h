#pragma once

#include <Gamnet/Library/Time/Time.h>
#include <map>
#include <Gamnet/Database/MySQL/Transaction.h>

class UserCount
{
public :
    enum Type
    {
        __Min__,
        __Max__,
    };

    typedef Gamnet::Time::DateTime DateTime;
    typedef Gamnet::Time::RepeatTimer RepeatTimer;

    class Meta
    {
    public :
        Type type;
        int flush_interval; // sec. if 0. immediately

        static bool Register(Type type, int interval);
        static std::shared_ptr<Meta> Find(Type type);

        static std::map<Type, std::shared_ptr<Meta>> metas;
    };
    
    struct Element
    {
        std::shared_ptr<Meta> meta;
        int64_t value;
        DateTime update_time;
    };

    class Insert : public Gamnet::Database::MySQL::Transaction::Query
    {
    public:
        Insert(std::shared_ptr<UserCount>& userCount, Type type);
     
        virtual Type GetType() const { return Gamnet::Database::MySQL::Transaction::Query::Type::Plain; }
        virtual std::string MakeQuery() override;
        virtual void Clear() override;

    private :
        std::shared_ptr<UserCount> userCount;
        Type type;
    };

    class Update : public Gamnet::Database::MySQL::Transaction::Query
    {
    public:
        Update(std::shared_ptr<UserCount>& userCount, );

        virtual Type GetType() const { return Gamnet::Database::MySQL::Transaction::Query::Type::Plain; }
        virtual std::string MakeQuery() override;
        virtual void Clear() override;

    private:
        std::shared_ptr<UserCount> userCount;
        Type type;
        
    };


    std::shared_ptr<Element> Create(Type type);
    void Remove(Type type);
    std::shared_ptr<Element> Find(Type type);

    std::shared_ptr<RepeatTimer> flush_timer;
private :
    std::map<Type, std::shared_ptr<Element>> elements;
};

#define USER_COUNT_REGISTER(type, interval) \
    static bool UserCountRegister##type = UserCount::Meta::Register(type, interval);
