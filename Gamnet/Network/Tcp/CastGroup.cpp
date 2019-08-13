#include "CastGroup.h"
#include "../../Library/Pool.h"

namespace Gamnet { namespace Network { namespace Tcp {

std::atomic<uint32_t> CastGroup::seq_generator;

CastGroup::CastGroup() : group_seq(0)
{
}

CastGroup::~CastGroup()
{
}

void CastGroup::AddSession(const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(lock);
	if (false == sessions.insert(std::make_pair(session->session_key, session)).second)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "castgroup_seq:", group_seq, ", duplicate session");
	}
}

void CastGroup::DelSession(const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(lock);
	sessions.erase(session->session_key);
}

void CastGroup::Clear()
{
	std::lock_guard<std::mutex> lo(lock);
	sessions.clear();
}

size_t CastGroup::Size()
{
	std::lock_guard<std::mutex> lo(lock);
	return sessions.size();
}

static Pool<CastGroup, std::mutex, CastGroup::Init> _castGroupPool(65535);

std::shared_ptr<CastGroup> CastGroup::Create()
{
	return _castGroupPool.Create();
}

}}}