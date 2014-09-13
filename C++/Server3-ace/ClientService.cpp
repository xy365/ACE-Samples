#include "stdafx.h"
#include "ClientService.h"

using namespace Bisque;

// Registers the new ClientService instance with the reactor
//
// We try to print a log message stating which host connected, then call ACE_Reactor::register_handler() 
// to register for input events with the reactor. We return the return value of register_handler(), 
// which is 0 for success and –1 for failure. This value is passed back to the ClientAcceptor::handle_input() method, 
// and handle_input() also will return –1 for error and 0 for success. 
// If ClientAcceptor::handle_input() returns –1 we will have to close the handle.
//
int ClientService::open()
{
	ACE_TCHAR		peerName[MAXHOSTNAMELEN];
	ACE_INET_Addr	peerAddress;

	if (this->m_socket.get_remote_addr(peerAddress) == 0 && peerAddress.addr_to_string(peerName, MAXHOSTNAMELEN) == 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), peerName));
	}

	return this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
}

// Called when this handler is removed from the ACE_Reactor
int ClientService::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
	if (mask == ACE_Event_Handler::WRITE_MASK)
	{
		return 0;
	}

	mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;

	this->reactor()->remove_handler(this, mask);
	this->m_socket.close();
	this->m_queue.flush();

	delete this;

	return 0;
}

// Called when input is available from the client
//
// We can handle requests until the peer closes on, potentially, many connections simultaneously.
// We do this asynchronously, acoiding blocking I/O operations which are bad because they block all connection processing. 
//
int ClientService::handle_input(ACE_HANDLE fileDescriptor)
{
	const size_t inputSize = 4096;
	char		 buffer[inputSize];
	ssize_t		 receivedBytes;
	ssize_t		 sentBytes;

	if ((receivedBytes = m_socket.recv(buffer, sizeof(buffer))) <= 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection closed\n")));
		return -1;
	}

	sentBytes = m_socket.send(buffer, static_cast<size_t>(receivedBytes));

	if (sentBytes == receivedBytes)
	{
		return 0;
	}

	if (sentBytes == -1 && ACE_OS::last_error() != EWOULDBLOCK)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("send")), 0);
	}

	if (sentBytes == -1)
	{
		sentBytes = 0;
	}

	//
	ACE_Message_Block* message;

	size_t remaining = static_cast<size_t>(receivedBytes - sentBytes);

	ACE_NEW_RETURN(message, ACE_Message_Block(remaining), -1);

	message->copy(&buffer[sentBytes], remaining);

	bool isEmpty = m_queue.is_empty();

	ACE_Time_Value nowait(ACE_OS::gettimeofday());

	if (m_queue.enqueue_tail(message, &nowait) == -1)
	{
		ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p; discarding data\n"), ACE_TEXT("enqueue failed")));
		message->release();
		return 0;
	}

	if (isEmpty)
	{
		return this->reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);		
	}

	return 0;
}

// Called when output is possible
int ClientService::handle_output(ACE_HANDLE fileDescriptor)
{
	ACE_Message_Block* message;
	ACE_Time_Value nowait(ACE_OS::gettimeofday());

	while (m_queue.dequeue_head(message, &nowait))
	{
		ssize_t sentBytes = m_socket.send(message->rd_ptr(), message->length());

		if (sentBytes == -1)
		{
			ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("send")));
		}
		else
		{
			message->rd_ptr(static_cast<size_t>(sentBytes));
		}

		if (message->length() > 0)
		{
			m_queue.enqueue_head(message);
			break;
		}

		message->release();
	}

	return (m_queue.is_empty() ? -1 : 0);
}
