#pragma once

#include <string>
#include <list>
#include <map>
#include <functional>
#include "../UserSession.h"
#include <Gamnet/Library/Variant.h>

class Cheat
{
public :
    typedef Gamnet::Variant Variant;
    typedef std::function<int(const std::shared_ptr<UserSession>&, const std::list<Variant>&)> Handler;

    void Init();

    void Register(const std::string& command, const Handler& handler);
    int Command(const std::string& command, std::shared_ptr<UserSession> session, const std::list<Variant>& params);

    int AddItem(const std::shared_ptr<UserSession>& session, const std::list<Variant>& params);

    std::map<std::string, Handler> handlers;
};