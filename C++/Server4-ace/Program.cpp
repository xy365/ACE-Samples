#include "stdafx.h"
#include "ClientService.h"

using namespace Bisque;

using std::cout;
using std::endl;

// Not needed with ClientAcceptor and ClientServiceEventHandler
typedef ACE_Acceptor<ClientService, ACE_SOCK_ACCEPTOR> ClientAcceptor;

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	//
	// We set up the ACE_INET_Addr object for the address at which to listen for connections. We instantiate a ClientAcceptor
	// object to represent our connection acceptor. ACE_Reactor::instance() obtains a pointer to a singleton ACE_Reactor instance. 
	// The vast majority of the time, this type of program uses one ACE_Reactor object to register all event handlers with 
	// and processes all the program’s events. Because this is so common, ACE provides a singleton access to an instance that ACE manages. 
	// When it’s first needed, it is created and is automatically shut down when the program finishes. It’s taken care of as part of the
	// ACE_Object_Manager. Because so much of what ACE_Event_Handler objects are used for is associated with ACE_Reactor, 
	// ACE_Event_Handler includes an ACE_Reactor pointer to conveniently refer to the reactor it’s using. This prevents building in 
	// assumptions or hard-coded references in your code to a particular ACE_Reactor instance that could cause maintenance problems in the future. 
	// Our example is implemented in a main program, and using the ACE_Reactor singleton is usually the best thing to do in this case. 
	// However, if this code was all included in a shared library, we would probably not want to hijack the use of the singleton, 
	// because it might interfere with a program using the shared library. We set the ACE_Reactor we want to use right at the start, 
	// and you’ll note that none of our classes refer directly to any ACE_Reactor instance directly. It’s all done via the 
	// ACE_Event_Handler::reactor() methods.
	//

	ACE_INET_Addr		listenAddress(27015, ACE_LOCALHOST);
	//ACE_INET_Addr		listenAddress("HAStatus");
	
	// Subscribe to events on a specific port
	ClientAcceptor acceptor;
	int hr = acceptor.open(listenAddress);

	if (hr == -1)
	{
		return 1;
	}

	cout << "Listening for incoming connections..." << endl;

	//
	// After establishing the ACE_Reactor to use, we call the acceptor object’s open() method. 
	// It will begin listening and register with the reactor for callbacks when new connections are ready to be accepted. 
	// The main program then simply enters the reactor event loop, run_reactor_event_loop(), which will continually 
	// handle events until an error occurs that prevents its further processing or a call is made to end_reactor_event_loop(). 
	// So, reactor based programs can spend quite a lot of their time waiting for something to do. When a client connects 
	// to our server’s listening port, the ACE_Reactor will detect an ACCEPT event and call back to the registered 
	// ClientAcceptor’s handle_input() method
	//

	ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;
}
