//---------------------------------------------------------------------------------------------------
// file ClientService.h
//
// Desc: Wrapper for ACE_SOCK_Stream. Services connection of a single client.
//
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ace\Null_Condition.h>
#include <ace\Message_Block.h>
#include <ace\Message_Queue.h>
#include <ace\SOCK_Stream.h>

namespace Bisque
{
	//
	// Commented out functions required with ACE_Event_Handler
	//

	class ClientService 
		: public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
		typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

	public:

		//
		// ACE_Event_Handler implementations
		//

		int					open			(void* p = nullptr);
		int					handle_close	(ACE_HANDLE handle, ACE_Reactor_Mask mask);					// Called when this handler is removed from the ACE_Reactor
		virtual int			handle_input	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);			// Called when input is available from the client
		virtual int			handle_output	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);			// Called when output is possible
	};
}
