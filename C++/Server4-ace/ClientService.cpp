#include "stdafx.h"
#include "ClientService.h"

using namespace Bisque;

// Registers the new ClientService instance with the reactor
//
// ACE_Svc_Handler::open() is to register the new handle for READ events. 
// And because ACE_Acceptor sets the reactor() pointer for each new service
// handler, that works just like we wanted, too, no matter which ACE_Reactor
// instance we use.
// If you ever need to know, the pointer passed into open() is a pointer to the
// ACE_Acceptor that created the handler object. The open() method is where
// the handler implements such things as its concurrency policy if it were, for
// example, to spawn a new thread or fork a new process. The default open()
// implementation does what we want: register the handler with its reactor for READ
// events. If open() returns –1, ACE_Acceptor will immediately call the
// handler’s close() hook method, a method inherited from ACE_Task. The
// default implementation will cause the handler to be deleted.
// Let’s now look at our new handle_input() method.
//
int ClientService::open(void* p)
{
	if (super::open(p) == -1)
	{
		return -1;
	}

	ACE_TCHAR		peerName[MAXHOSTNAMELEN];
	ACE_INET_Addr	peerAddress;

	if (this->peer().get_remote_addr(peerAddress) == 0 && peerAddress.addr_to_string(peerName, MAXHOSTNAMELEN) == 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), peerName));
	}

	return 0;
}

// Called when this handler is removed from the ACE_Reactor
int ClientService::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
	if (mask == ACE_Event_Handler::WRITE_MASK)
	{
		return 0;
	}

	return super::handle_close(handle, mask);
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

	receivedBytes = this->peer().recv(buffer, sizeof(buffer));

	if (receivedBytes <= 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection closed\n")));
		return -1;
	}

	sentBytes = this->peer().send(buffer, static_cast<size_t>(receivedBytes));

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

	bool isEmpty = this->msg_queue()->is_empty();

	ACE_Time_Value nowait(ACE_OS::gettimeofday());

	if (this->putq(message, &nowait) == -1)
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

	while (this->getq(message, &nowait))
	{
		ssize_t sentBytes = this->peer().send(message->rd_ptr(), message->length());

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
			this->ungetq(message);
			break;
		}

		message->release();
	}

	return (this->msg_queue()->is_empty() ? -1 : 0);
}
