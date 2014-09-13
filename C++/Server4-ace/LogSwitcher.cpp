#include "stdafx.h"
#include <ace\Signal.h>
#include "LogSwitcher.h"

using namespace Bisque;

LogSwitcher::LogSwitcher(int on, int off)
	: m_on(on)
	, m_off(off)
{
	ACE_Sig_Set list;

	list.sig_add(on);
	list.sig_add(off);

	ACE_Reactor::instance()->register_handler(list, this);
}

// Called when object is signalled by OS
//
// handle_signal() method checks whether it should turn logging on or off 
// and then calls notify() to queue a callback to its own handle_exception() method. 
// (EXCEPT_MASK is the default event type; because we wanted that one,  we left the 
// argument off the notify() call.) After returning to the reactor, the reactor will 
// complete its handling of signals and return to waiting for events. Shortly thereafter, 
// it will notice the queued notification event and dispatch control to handle_exception():
//
int LogSwitcher::handle_signal(int signal, siginfo_t*, ucontext_t*)
{
	if (signal == m_on || signal == m_off)
	{
		m_switch = (signal == m_on);
		ACE_Reactor::instance()->notify(this);
	}

	return 0;
}

// Called when an exception occurs
//
// The handle_exception() method examines the saved state to see what
// action it should take and sets or clears the SILENT flag.
//
int LogSwitcher::handle_exception(ACE_HANDLE fileDescriptor)
{
	if (m_switch)
	{
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::SILENT);
	}
	else
	{
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT);
	}

	return 0;
}
