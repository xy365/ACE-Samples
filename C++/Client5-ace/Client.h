#pragma once

#include "stdafx.h"

namespace Bisque
{
	class Client : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
		typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

	public:
		Client();

		//
		// ACE_Svc_Handler implementations
		//

		virtual int open			(void* p = 0);
		virtual int handle_input	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);							// Called when input is available
		virtual int handle_output	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);							// Called when output is possible
		virtual int handle_timeout	(const ACE_Time_Value& currentTime, const void* completionToken = 0);		// Called when a timer expires

	private:
		int									m_numIterations;
		ACE_Reactor_Notification_Strategy	m_notifier;

		static const int MaxIterations = 5;
	};
}
/*
That code is standard stuff. (ACE is like that, by the way: very consistent.
Once you learn how to do something, the knowledge carries well.) However, we
encounter our first new feature. ACE_Reactor_Notification_Strategy
is a class in the strategies category and implements the Strategy pattern [3],
which allows you to customize another class’s behavior without changing the
subject class. Let’s look a little more closely at what it does; it’s used in the
open() method:
int Client::open (void *p)
{
ACE_Time_Value iter_delay (2); // Two seconds
if (super::open (p) == -1)
return -1;
*/