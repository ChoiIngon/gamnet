#include "UserCount.h"

import Gamnet.String;

std::map<UserCount::Type, std::shared_ptr<UserCount::Meta>>UserCount::Meta::metas;

bool UserCount::Meta::Register(UserCount::Type type, int interval)
{
    auto itr = metas.find(type);
    if (metas.end() != itr)
    {
        assert(false);
        return false;
    }

    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    meta->type = type;
    meta->flush_interval = interval;
    metas.insert(std::make_pair(type, meta));
    return true;
}

std::shared_ptr<UserCount::Meta> UserCount::Meta::Find(Type type)
{
    auto itr = metas.find(type);
    if (metas.end() == itr)
    {
        return nullptr;
    }

    return itr->second;
}

std::shared_ptr<UserCount::Data> UserCount::Create(Type type)
{
    auto meta = UserCount::Meta::Find(type);
    if (nullptr == meta)
    {
        return nullptr;
    }

    std::shared_ptr<UserCount::Data> data = std::make_shared<UserCount::Data>();
    data->meta = meta;
    data->value = 0;
    data->update_time = Gamnet::Time::Local::Now();

    datas.insert(std::make_pair(type, data));

    return data;
}

std::shared_ptr<UserCount::Data> UserCount::Find(Type type)
{
    auto itr = datas.find(type);
    return itr->second;
}

void UserCount::Remove(Type type)
{
    datas.erase(type);
}

UserCount::Insert::Insert(const std::shared_ptr<Session>& session, UserCount::Type type)
    : session(session)
    , type(type)
{
    std::shared_ptr<UserCount> pUserCount = session->GetComponent<UserCount>();
    this->data = pUserCount->Create(type);
}
 
std::string 
UserCount::Insert::Commit()
{
    return Gamnet::Format(
        "INSERT INTO `user_count`(`user_no`, `type`, `value`, `update_time`)"
        " VALUES (",
            session->user_no, ",",
            data->meta->type, ",",
            data->value, ",",
            data->update_time, 
        ")"
    );
}

void UserCount::Insert::Rollback()
{
    std::shared_ptr<UserCount> pUserCount = session->GetComponent<UserCount>();
    pUserCount->Remove(type);
}