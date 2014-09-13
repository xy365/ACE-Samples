//---------------------------------------------------------------------------------------------------
// file LoopStopper.h
//
// Desc: The LoopStopper class registers the single specified signal with the reactor singleton. 
//       When the signal is caught, the reactor calls the handle_signal() callback method, 
//       which simply calls the end_reactor_event_loop() method on the reactor singleton. 
//       On return to the event loop, the event loop will end. If we instantiate one of 
//		 these objects in the server’s main program, we will quickly and easily add 
//		 the ability to shut the server down cleanly by sending it a SIGINT signal.
//
//---------------------------------------------------------------------------------------------------

#pragma once

namespace Bisque
{
	class LoopStopper : public ACE_Event_Handler
	{
	public:

		//
		// ACE_Event_Handler implementations
		//
		LoopStopper(int signal = SIGINT);

		virtual int handle_signal(int signal, siginfo_t* = nullptr, ucontext_t* = nullptr);			// Called when object is signalled by OS
	};
}
