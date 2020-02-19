#include "CastGroup.h"
#include "../../Library/Pool.h"


namespace Gamnet { namespace Network { namespace Tcp {

static std::atomic<uint32_t> CAST_GROUP_SEQ;

CastGroup* CastGroup::Init::operator() (CastGroup* group)
{
	group->group_seq = ++CAST_GROUP_SEQ;
	return group;
}

CastGroup::CastGroup() : group_seq(0)
{
}

CastGroup::~CastGroup()
{
}

size_t CastGroup::AddSession(const std::shared_ptr<Session>& session)
{
	if (false == sessions.insert(std::make_pair(session->session_key, session)).second)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "castgroup_seq:", group_seq, ", duplicate session");
	}
	return sessions.size();
}

size_t CastGroup::DelSession(const std::shared_ptr<Session>& session)
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

void CastGroup::lock() 
{
	_lock.lock();
}

bool CastGroup::try_lock() 
{ 
	return true; 
}

void CastGroup::unlock()
{
	_lock.unlock();
}

static Pool<CastGroup, std::mutex, CastGroup::Init> _castGroupPool(65535);

std::shared_ptr<CastGroup> CastGroup::Create()
{
	return _castGroupPool.Create();
}

}}}