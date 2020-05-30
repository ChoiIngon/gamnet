#include "SessionManager.h"
#include "../Network/Tcp/SystemMessageHandler.h"

namespace Gamnet { namespace Test {
	void SessionManagerImpl::Init(const char* host_, int port_, int sessionCount_, int loopCount_)
	{
		log_timer = Time::Timer::Create();
		log_timer->AutoReset(true);
		log_timer->SetTimer(5000, std::bind(&SessionManagerImpl::OnLogTimerExpire, this));

		log.Init("test", "test", 5);
		if (0 == sessionCount_)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidArgumentError, " 'session_count' should be set");
		}

		host = host_;
		port = port_;
		begin_execute_count = 0;
		finish_execute_count = 0;
		session_count = sessionCount_;
		max_execute_count = sessionCount_ * loopCount_;
	}

	void SessionManagerImpl::OnLogTimerExpire()
	{
		//log.Write(GAMNET_INF, "[Test] link count..(active:", this->Size(), ", available:", link_pool.Available(), ", max:", link_pool.Capacity(), ")");
		//log.Write(GAMNET_INF, "[Test] session count..(active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");
		log.Write(GAMNET_INF, "[Test] begin count..(", begin_execute_count, "/", max_execute_count, ")");

		if (finish_execute_count >= max_execute_count)
		{
			log_timer->Cancel();
			log.Write(GAMNET_INF, "[Test] test finished..(", finish_execute_count, "/", max_execute_count, ")");
		}
	}
}}