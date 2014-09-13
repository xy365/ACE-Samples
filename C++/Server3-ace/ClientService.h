//---------------------------------------------------------------------------------------------------
// file ClientService.h
//
// Desc: Wrapper for ACE_SOCK_Stream. Services connection of a single client.
//		 Will be later replaced by ClientService class using Acceptor pattern.
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

	class ClientService : public ACE_Event_Handler
	{
	public:

		//
		// ACE_Event_Handler implementations
		//

		int					open			();
		int					handle_close	(ACE_HANDLE handle, ACE_Reactor_Mask mask);					// Called when this handler is removed from the ACE_Reactor
		virtual int			handle_input	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);			// Called when input is available from the client
		virtual int			handle_output	(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);			// Called when output is possible

		ACE_SOCK_Stream&	peer			()		 { return m_socket; }
		virtual ACE_HANDLE	get_handle		() const { return m_socket.get_handle(); }					// Handler's I/O handle

	protected:
		ACE_SOCK_Stream						m_socket;
		ACE_Message_Queue<ACE_NULL_SYNCH>	m_queue;
	};
}
