//---------------------------------------------------------------------------------------------------
// file LogSwitcher.h
//
// Desc: This initially looks very similar to the LoopStopper class but with some added data 
//       to be able to say which signal turns logging on and which one turns it off. 
//		 We show the register_handler() variant that registers a whole set of signals at once.
//		 The advantages of using ACE_Sig_Set instead of multiple calls to register_handle() 
//		 may not be immediately apparent, but it is generally easier to work with a set, 
//		 or collection, of things than with a number of individual items. For instance, 
//		 you can create the signal set in an initialization routine, pass it around 
//		 for a while, and then feed it to the reactor.
//
//---------------------------------------------------------------------------------------------------

#pragma once

namespace Bisque
{
	class LogSwitcher : public ACE_Event_Handler
	{
	public:

		//
		// ACE_Event_Handler implementations
		//

		LogSwitcher(int on, int off);

		virtual int handle_signal	(int signal, siginfo_t* = nullptr, ucontext_t* = nullptr);			// Called when object is signalled by OS
		virtual int handle_exception(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);					// Called when an exception occurs

	private:
		LogSwitcher() {};

		int  m_on;				// Signal to turn logging on
		int  m_off;				// Signal to turn logging off
		bool m_switch;			// true == on, false == off
	};
}
