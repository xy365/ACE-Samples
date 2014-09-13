#include "ClientAcceptor.h"
#include "ClientService.h"

using namespace Bisque;

using std::cout;
using std::endl;

ClientAcceptor::~ClientAcceptor(void)
{
	// Ensures that there’s no dangling reactor registration
	this->handle_close(ACE_INVALID_HANDLE, 0);
}

// Registers handle with ACE Reactor to listen for connection events.
// Opens ACE_SOCK_Acceptor to begin listening at the requested listen address. 
// The code does not do a blocking accept() call. Instead, it calls ACE_Reactor::register_handler(), 
// which tells the reactor to watch for ACCEPT events on the acceptor’s handle. Whenever those
// events occur, Reactor calls back to this object. 
// Because we didn’t specify the handle value in the register_handler() call, the Reactor calls back to the
// get_handle() method of the object being registered which returns the ACE_SOCK_Acceptor’s handle value.
//
int ClientAcceptor::open(const ACE_INET_Addr& listenAddress)
{
	// In addition to the address at which we hope to receive connection requests,
	// we’ve also given the acceptor’s open() method a reuse_addr flag (the second argument) set to 1. 
	// Setting the flag this way will allow our acceptor to open even if some sockets are already connected 
	// at our designated listen port, probably left over from a previous execution of the server. 
	// This is generally what you want to do, because even sockets in the FIN_WAIT state can prevent 
	// an acceptor from opening successfully.

	int hr = m_acceptor.open(listenAddress, 1);

	if (hr == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p acceptor.open\n")), -1);
	}

	return this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

// Called when a connection is ready to accept
//
// Although it is usually much more straightforward to associate a single I/O handle with each handler, 
// there’s no restriction about that. An I/O handler can be registered for multiple I/O handles. 
// The particular handle that triggered the callback is passed to handle_input(). Our example is concerned 
// only with one ACE_SOCK_Acceptor; therefore, we ignore the ACE_HANDLE parameter of our handle_input() method.
// The first thing our handle_input() method does is create a ClientService instance. Because we made a decision 
// to use a separate service handler object for each connection, each new connection acceptance gets a new ClientService instance.
// We need access to the ACE_SOCK_Stream which we wrapped in the ClientService class. Therefore, ClientService offers 
// a peer() method that returns a reference to its ACE_SOCK_Stream object. If we succeed in our attempt to accept 
// the client’s connection, we will pass our ACE_Reactor pointer along to the new event handler in case it needs it 
// and then notify the ClientService that it should get ready to do some work. This is done through the open() method. 
// Our example’s open() method registers the new ClientService instance with the reactor.
//
int ClientAcceptor::handle_input(ACE_HANDLE fileDescriptor)
{
	ClientService* client;
	ACE_NEW_RETURN(client, ClientService, -1);
	auto_ptr<ClientService> p(client);

	if (m_acceptor.accept(client->peer()) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("Failed to accept client connection")), -1);		
	}

	p.release();
	client->reactor(this->reactor());

	if (client->open() == -1)
	{
		client->handle_close(ACE_INVALID_HANDLE, 0);
	}

	return 0;
}

// Called when the handle is removed from ACE_Reactor
int ClientAcceptor::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
	if (m_acceptor.get_handle() != ACE_INVALID_HANDLE)
	{
		ACE_Reactor_Mask mask = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
		this->reactor()->remove_handler(this, mask);
		m_acceptor.close();
	}

	return 0;
}
