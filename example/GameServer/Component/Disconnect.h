#ifndef _COMPONENT_DISCONNECT_H_
#define _COMPONENT_DISCONNECT_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/Time.h>
#include <Gamnet/Library/Time/Timer.h>

namespace Component {
	class Disconnect
	{
	public:
		Disconnect();

		Gamnet::Time::Timer timer;
		Gamnet::Time::ElapseTimeCounter elapse_time;
	};
};
#endif
