#include "AsyncResponse.h"

namespace Gamnet { namespace Network { namespace Tcp {

std::atomic_uint32_t response_seq;

IAsyncResponse::IAsyncResponse()
	: seq(0)
{
}

IAsyncResponse::~IAsyncResponse()
{
}

void IAsyncResponse::Init()
{
	seq = ++response_seq;
}

void IAsyncResponse::Clear()
{
	seq = 0;
}

}}}