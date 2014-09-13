//---------------------------------------------------------------------------------------------------
// file TimerHandler.h
//
// Desc: All handlers registered with the reactor share the same process/thread. As such, the time 
//		 at which handle_timeout() is invoked might not be the time at which the timer expired. 
//		 That is, the interval timer for a handler may expire while another event handler is in the 
//		 middle of one of its callback methods. The current_time parameter is the time that 
//		 our event handler was selected for dispatching rather than the current system time.
//		 Registering your timer handler with the reactor is straightforward: 
//
//		 TimerHandler * timer = new TimerHandler ();
//		 ACE_Time_Value initialDelay (3);
//		 ACE_Time_Value interval (5);
//		 ACE_Reactor::instance()->schedule_timer (timer, 0, initialDelay, interval);
//
//		 We’ve set an initial delay of 3 seconds and an interval of 5 seconds.
//
//		 If you use the thread-pool reactor (ACE_TP_Reactor), the handlers share a pool of threads
//		 rather than just one. The current_time is still important, however, because you will likely
//		 register more handlers than the number of threads in the pool. Because of this shared nature, 
//		 if you have timers that must be handled at exactly the timeout interval, a dedicated thread or
//		 process might be your only option.
//
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ace\OS_NS_time.h>

namespace Bisque
{
	class TimerHandler : public ACE_Event_Handler
	{
	public:

		//
		// ACE_Event_Handler implementations
		//

		int handle_timeout(const ACE_Time_Value& currentTime, const void* = nullptr)
		{
			time_t epoch = (static_cast<timespec_t>(currentTime)).tv_sec;

			ACE_DEBUG((LM_INFO, ACE_TEXT("handle_timeout: %s\n"), ACE_OS::ctime(&epoch)));

			return 0;
		}
	};
}
