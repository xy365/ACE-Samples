//---------------------------------------------------------------------------------------------------
// UDP Receiver
//
// Receives and replays UDP DGRAM in unicast mode
//
//---------------------------------------------------------------------------------------------------

//#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\Log_Msg.h>
#include <ace\SOCK_Dgram.h>

//#pragma warning(default:4996)

#include <iostream>

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

using std::cout;
using std::endl;

void EchoDatagram()
{
	ACE_INET_Addr  listener(27015, ACE_LOCALHOST);
	ACE_INET_Addr  sender;
	ACE_SOCK_DGRAM udp(listener);

	char buffer[BUFSIZ];
	size_t size = sizeof(buffer);

	cout << "waiting for a udp datagram." << endl;

	ssize_t received = udp.recv(buffer, size, sender);

	if (received > 0)
	{
		udp.send(buffer, size, sender);
	}

	udp.close();

	cout << "received: " << received << " bytes" << endl;
}

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	EchoDatagram();
	return 0;
}
