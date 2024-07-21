#include "UserCount.h"


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

std::shared_ptr<UserCount::Element> UserCount::Create(Type type)
{
    auto meta = UserCount::Meta::Find(type);
    if (nullptr == meta)
    {
        return nullptr;
    }

    std::shared_ptr<Element> elmt = std::make_shared<Element>();
    elmt->meta = meta;
    elmt->value = 0;
    elmt->update_time = Gamnet::Time::Local::Now();

    elements.insert(std::make_pair(type, elmt));

    return elmt;
}


 