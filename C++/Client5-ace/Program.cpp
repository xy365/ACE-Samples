#include "stdafx.h"
#include "Client.h"

using namespace Bisque;

//
// Usually it is better to allocate all service handlers dynamically. 
// In this example below, we could have set p to nullptr, and ACE_Connector 
// would have dynamically allocated a Client for the new connection. 
// Default ACE_Svc_Handler::handle_close() method unregisters the handler 
// with the reactor, cancels all the timers, and deletes the object. 
// In reality, the method deletes the handler object only if it was dynamically allocated;
// if it wasn’t dynamically allocated, handle_close() doesn’t try to delete it.
// How does the method know whether the handler was dynamically allocated? 
// It uses the C++ Storage Class Tracker idiom. C++ Network Programming,
// Volume 2 [7] has a good description of how ACE_Svc_Handler uses this technique internally.
//
int ACE_TMAIN(int, ACE_TCHAR*[])
{
	ACE_INET_Addr server(27015/*"HAStatus"*/, ACE_LOCALHOST);
	ACE_Connector<Client, ACE_SOCK_CONNECTOR> socket;

	Client client;
	Client* p = &client;

	if (socket.connect(p, server) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("connect")), 1);
	}

	ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;
}
