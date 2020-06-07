#include "CastGroup.h"

#include <atomic>
#include "../../Library/Pool.h"


namespace Gamnet { namespace Network { namespace Tcp {

static std::atomic<uint32_t> CAST_GROUP_SEQ;

CastGroup* CastGroup::Init::operator() (CastGroup* group)
{
	group->group_seq = ++CAST_GROUP_SEQ;
	return group;
}

CastGroup::LockGuard::LockGuard(const std::shared_ptr<CastGroup>& obj) : ptr(obj)
{
	ptr->lock.lock();
}

CastGroup::LockGuard::~LockGuard()
{
	ptr->lock.unlock();
}

std::shared_ptr<CastGroup> CastGroup::LockGuard::operator -> ()
{
	return ptr;
}

CastGroup::CastGroup() : group_seq(0)
{
}

CastGroup::~CastGroup()
{
}

size_t CastGroup::Insert(const std::shared_ptr<Session>& session)
{
	if (false == sessions.insert(std::make_pair(session->session_key, session)).second)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "castgroup_seq:", group_seq, ", duplicate session");
	}
	return sessions.size();
}

size_t CastGroup::Remove(const std::shared_ptr<Session>& session)
{
	sessions.erase(session->session_key);
	return sessions.size();
}

void CastGroup::Clear()
{
	sessions.clear();
}

size_t CastGroup::Size()
{
	return sessions.size();
}

static Pool<CastGroup, std::mutex, CastGroup::Init> _castGroupPool(10);

std::shared_ptr<CastGroup> CastGroup::Create()
{
	return _castGroupPool.Create();
}

}}}