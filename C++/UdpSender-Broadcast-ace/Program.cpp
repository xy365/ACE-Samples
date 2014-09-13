//---------------------------------------------------------------------------------------------------
// UDP Sender: Broadcast
//
// Sends UDP DGRAM in broadcast mode
//
// In broadcast mode, the destination address must be specified for each send
// operation. However, the UDP port number part is all that changes between sends,
// because the IP address part is always the IP broadcast address, which is a fixed
// value. The ACE_SOCK_Dgram_Bcast class takes care of supplying the correct
// IP broadcast address for you; you need specify only the UDP port number to
// broadcast to.
//
// The ACE_SOCK_Dgram_Bcast class is a subclass of ACE_SOCK_Dgram, so all datagram 
// receive operations are similar to those in the unicast examples.
//
//---------------------------------------------------------------------------------------------------

//#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\Log_Msg.h>
#include <ace\OS.h>
#include <ace\SOCK_Dgram_Bcast.h>

//#pragma warning(default:4996)

#include <iostream>

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

using std::cout;
using std::endl;

int SendBroadcast()
{
	const char* message = "this is a message!\n";

	ACE_INET_Addr		 sender	(27016, ACE_LOCALHOST);
	ACE_SOCK_Dgram_Bcast udp	(sender);

	unsigned short listenerPort = 27015;

	ssize_t sent = udp.send(message, ACE_OS_String::strlen(message) + 1, listenerPort);

	udp.close();

	if (sent == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("send")), -1);
	}

	cout << "sent:     " << sent << " bytes" << endl;

	return 0;
}

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	int hr = SendBroadcast();
	return hr;
}
