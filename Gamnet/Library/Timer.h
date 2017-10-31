#ifndef __GAMNET_LIB_TIMER_H_
#define __GAMNET_LIB_TIMER_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include "Singleton.h"

namespace Gamnet
{
/*!
 * \brief 이벤트 타이머

		    클래스 생성 혹은 초기화 함수를 이용하여 지정된 시간이 경과하면, < br>
		    등록된 콜백 함수를 이용하여 어플리케이션에게 이벤트를 알린다.

 * 		 Usage :
 * 		 <ol>
 * 		  <li> 타이머(Timer) 객체 생성. <br>
 * 		  	      환경에 따라 single thread or multi thread 용으로 선언한다.
 *     	  <li> SetTimer 함수를 이용하여 expire 이벤트가 발생할 시간을 지정한다.
 *         <li> 반복 적인 이벤트 발생이 필요하면 Resume() 함수를 호출 한다.
 * 		 </ol>
 * 		 Sample code :<br>
 * 	<pre>
	Toolkit::ThreadPool threadPool(10);
	Toolkit::Timer timer;
	timer.SetTimer(&threadPool, 500, [&timer](){
 *		// Do Something
 *		timer.Resume();
	});
 * 	</pre>
 */
struct Timer
{
	struct TimerEntry
	{
		virtual ~TimerEntry()
		{
		}
		virtual void OnExpire() = 0;
	};

	template<class FUNCTOR>
	struct TimerEntryT : public TimerEntry
	{
	    FUNCTOR functor_;
	    TimerEntryT(FUNCTOR functor) : functor_(functor) {}
	    virtual ~TimerEntryT() {}
	    virtual void OnExpire() { functor_();}
	};

	std::shared_ptr<TimerEntry> entry_;
	long interval_;
	boost::asio::deadline_timer deadline_timer_;

	Timer() :
		entry_(NULL), interval_(0), deadline_timer_(Singleton<boost::asio::io_service>::GetInstance())
	{
	}

	/*!
		\param threadPool 타이머 이벤트 발생 시 지정된 함수자를 실행하기 위한 thread pool
		\param interval ms단위(1/1000초) 단위. 함수 호출 이후 interval 만큼 시간이 흐르면 등록된 함수자가 호출 된다.(100ms 정밀도를 가진다)
		\param functor 타이머 이벤트 발생 시 수행될 함수자. std::bind를 이용하여 어떠한 함수라도 전달 가능하다.
	*/
	template <class FUNCTOR>
	Timer(long interval, FUNCTOR functor) :
		entry_(NULL), interval_(interval), deadline_timer_(Singleton<boost::asio::io_service>::GetInstance())
	{
		SetTimer(interval, functor);
	}

	/*!
	  \param threadPool 타이머 이벤트 발생 시 지정된 함수자를 실행하기 위한 thread pool
	  \param interval ms단위(1/1000초) 단위. 함수 호출 이후 interval 만큼 시간이 흐르면 등록된 함수자가 호출 된다.(100ms 정밀도를 가진다)
	  \param functor 타이머 이벤트 발생 시 수행될 함수자. std::bind를 이용하여 어떠한 함수라도 전달 가능하다.
	 */
	template <class FUNCTOR>
	bool SetTimer(int interval, FUNCTOR functor)
	{
		interval_ = interval;
		entry_ = std::shared_ptr<TimerEntryT<FUNCTOR>>(new TimerEntryT<FUNCTOR>(functor));
		deadline_timer_.expires_from_now(boost::posix_time::milliseconds(interval_));
		deadline_timer_.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));

		return true;
	}
	void OnExpire(const boost::system::error_code& ec)
	{
		if(0 != ec)
		{
			return ;
		}
		entry_->OnExpire();
	}
    /*!
     * \brief 타이머 이벤트는 1회성이다. 지속적으로 발생 시키기 위해서는 필요할때 마다 Resume() 함수를 호출 해야 한다.
     */
	bool Resume()
	{
		deadline_timer_.expires_at(deadline_timer_.expires_at() + boost::posix_time::milliseconds(interval_));
		deadline_timer_.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
		return true;
	}

	virtual ~Timer()
	{
		deadline_timer_.expires_at(boost::posix_time::pos_infin);
		entry_ = nullptr;
	}

	void Cancel()
	{
		deadline_timer_.cancel();
		entry_ = nullptr;
	}
};

};

#endif /* TIMER_H_ */
