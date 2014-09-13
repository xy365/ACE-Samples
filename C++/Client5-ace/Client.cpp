#include "stdafx.h"
#include "Client.h"

using namespace Bisque;

Client::Client(void)
	: m_notifier		(0, this, ACE_Event_Handler::WRITE_MASK)
	, m_numIterations	(0)
{
}

// Opens connection
//
// ACE_Reactor_Notification_Strategy is a class in the strategies category 
// and implements the Strategy pattern which allows to customize another class’s 
// behavior without changing the subject class.
//
// First, we call the superclass’s open() method to register this handler with the
// reactor for READ events. As with ACE_Acceptor, ACE_Connector automatically
// calls the handler’s open() hook method when the connection is established.
// As with ACE_Acceptor, if open() returns –1, close() will be called immediately. 
// If that goes well, we set up the notification strategy on our inherited 
// ACE_Message_Queue, using its notification_strategy() method. We set up 
// the strategy object to specify this handler and the WRITE_MASK. The 0 was 
// a reactor pointer. At the constructor point, we didn’t know which ACE_Reactor 
// instance would be used with this Client object, so we left it 0. We now know it, 
// as ACE_Connector assigns it before calling the open() method. So we set the correct
// ACE_Reactor pointer in our notifier_ object. What is all this for? The ACE_Message_Queue 
// can be strategized with an object, such as ACE_Reactor_Notification_Strategy. 
// When it has one of these strategy objects, ACE_Message_Queue calls the strategy’s 
// notify() method whenever an ACE_Message_Block is enqueued. As we’ve set up m_notifier, 
// it will do a notify() call on the Client’s reactor to queue a notification for 
// handle_output() in our Client object. This facility is very useful for incorporating 
// ACE_Message_Queue enqueue operations into a reactor’s event loop. The last thing our 
// open() method does is set up a recurring timer for now and every 2 seconds after that.
//
int Client::open(void* p)
{
	ACE_Time_Value delay(2);		// 2 seconds

	if (super::open(p) == -1)
	{
		return -1;
	}

	this->m_notifier.reactor(this->reactor());
	this->msg_queue()->notification_strategy(&this->m_notifier);

	return (this->reactor()->schedule_timer(this, 0, ACE_Time_Value::zero, delay));
}

// Called when input is available
//
// Our server example will simply echo back whatever it receives from the client.
// If we receive data, we log it, this time using a counted string, as the 
// received data does not have a 0 terminator byte. If there’s an error, 
// we end the event loop and return –1 to remove this handler from the reactor.
//
int Client::handle_input(ACE_HANDLE fileDescriptor)
{
	char buffer[64];
	ssize_t received = this->peer().recv(buffer, sizeof(buffer) - 1);

	if (received > 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("%.*C"), static_cast<int>(received), buffer));
		return 0;
	}

	if (received == 0 || ACE_OS::last_error() != EWOULDBLOCK)
	{
		this->reactor()->end_reactor_event_loop();
		return -1;
	}

	return 0;
}

// Called when output is possible
//
// We continue to dequeue and send data until the queue is empty or the socket
// becomes flow controlled and we have to wait for more. To manage the callbacks
// to this method, we use two alternative reactor registration methods:
// cancel_wakeup() and schedule_wakeup(). Each assumes that the handler being 
// specified is already registered with the reactor for at least one other I/O 
// event type. (Client is registered for READ events from the open() method.) 
// cancel_wakeup() removes the specified mask bit(s) from this handler’s reactor 
// registration, and schedule_wakeup() adds the specified mask. The handler is not 
// added or removed in either case, so there’s no handle_close() call as a result 
// of removing WRITE_MASK. Therefore, we don’t need to implement handle_close() 
// this time to special-case handling of a closed WRITE mask; we reuse the default 
// handle_close() method from ACE_Svc_Handler.
//
int Client::handle_output(ACE_HANDLE fileDescriptor)
{
	ACE_Message_Block*  message;
	ACE_Time_Value		noWait(ACE_OS::gettimeofday());

	while (this->getq(message, &noWait) != -1)
	{
		ssize_t sent = this->peer().send(message->rd_ptr(), message->length());

		if (sent == -1)
		{
			ACE_ERROR((LM_ERROR, ACE_TEXT("%P|%t) %p\n"), ACE_TEXT("send")));
		}
		else
		{
			message->rd_ptr(static_cast<size_t>(sent));
		}

		if (message->length() > 0)
		{
			this->ungetq(message);
			break;
		}

		message->release();
	}

	if (this->msg_queue()->is_empty())
	{
		this->reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);
	}
	else
	{
		this->reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);
	}

	return 0;
}

// Called when a timer expires
//
// After we send a predetermined number of strings to the server, we use the close_writer() 
// method to close our end of the TCP/IP socket. This will cause the server to see that 
// we’ve closed, and it, in turn, will close its end, and we’ll end up back in handle_input(), 
// doing a 0-byte receive to close everything down.
//
// Note that for each string we want to send to the server, we insert it into a ACE_Message_Block 
// and enqueue it. This will cause the message queue to use our m_notifier object to queue a 
// notification to the reactor; when it’s processed, the reactor will call our handle_output() method.
//
// completionToken is an asynchronous completion token, it is used to efficiently match up
// asynchronous I/O opertions with their completion events
//
int Client::handle_timeout(const ACE_Time_Value& currentTime, const void* completionToken)
{
	if (++m_numIterations >= MaxIterations)
	{
		this->peer().close_writer();
		return 0;
	}

	ACE_Message_Block* message;

	ACE_NEW_RETURN(message, ACE_Message_Block(128), -1);

	int bytes = ACE_OS::sprintf(message->wr_ptr(), "Iteration %d\n", m_numIterations);

	ACE_ASSERT(bytes > 0);

	message->wr_ptr(static_cast<size_t>(bytes));

	this->putq(message);

	return 0;
}
