#include "stdafx.h"
#include "LoopStopper.h"

using namespace Bisque;

LoopStopper::LoopStopper(int signal)
{
	ACE_Reactor::instance()->register_handler(signal, this);
}

int LoopStopper::handle_signal(int, siginfo_t*, ucontext_t*)
{
	ACE_Reactor::instance()->end_reactor_event_loop();
	return 0;
}
