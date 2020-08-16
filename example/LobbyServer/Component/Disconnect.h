#ifndef _COMPONENT_DISCONNECT_H_
#define _COMPONENT_DISCONNECT_H_

#include "../../idl/MessageCommon.h"
#include <Gamnet/Library/Time/Time.h>

namespace Component {
	class Disconnect
	{
	public:
		Disconnect();

		Gamnet::Time::ElapseTimer elapse_time;
	};
};
#endif
