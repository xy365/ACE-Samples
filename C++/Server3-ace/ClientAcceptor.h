//---------------------------------------------------------------------------------------------------
// file ClientAcceptor.h
//
// Desc: Accepts connections from clients
//
//---------------------------------------------------------------------------------------------------

#pragma once

#include "stdafx.h"

namespace Bisque
{
	class ClientAcceptor : public ACE_Event_Handler
	{
	public:
		virtual ~ClientAcceptor(void);

		//
		// ACE_Event_Handler implementations
		//
		int open(const ACE_INET_Addr& listenAddress);

		// Called when a connection is ready to accept
		virtual int handle_input(ACE_HANDLE fileDescriptor = ACE_INVALID_HANDLE);

		// Called when the handle is removed from ACE_Reactor
		virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask);

		// Returns I/O handle: note that the name get_handle is must not be changed because it is used as a default callback.
		virtual ACE_HANDLE get_handle() const { return m_acceptor.get_handle(); }

	protected:
		ACE_SOCK_Acceptor m_acceptor;			// I/O handle
	};
}
