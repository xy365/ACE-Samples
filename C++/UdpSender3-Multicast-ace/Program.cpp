//---------------------------------------------------------------------------------------------------
// UDP Sender: Multicast
//
// Sends UDP DGRAM in multicast mode
//
// UDP multicast mode involves a group of network nodes called a multicast group.
// The underlying OS-supplied protocol software manages multicast groups by using
// specialized protocols. The OS directs the group operations based on applications’
// requests to join—subscribe to—or leave—unsubscribe from—a particular multicast
// group. Once an application has joined a group, all datagrams sent on the
// joined socket are sent to the multicast group without specifying the destination
// address for each send operation.
//
// Each multicast group has a separate IP address. Multicast addresses are IP
// class D addresses, which are separate from the class A, B, and C addresses that
// individual host interfaces are assigned. Applications define and assign class D
// addresses specific to the application.
//
// The following example shows how to join a multicast group and transmit a
// datagram to the group, using the ACE_SOCK_Dgram_Mcast class.
//
//---------------------------------------------------------------------------------------------------

//#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\Log_Msg.h>
#include <ace\OS.h>
#include <ace\SOCK_Dgram_Mcast.h>

//#pragma warning(default:4996)

#include <iostream>

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

using std::cout;
using std::endl;

int SendMulticast()
{
	const char* message = "this is a message!\n";

	ACE_INET_Addr		 listener;// (27015, ACE_LOCALHOST);
	ACE_INET_Addr		 sender	(27016, ACE_LOCALHOST);
	ACE_SOCK_Dgram_Mcast udp;

	int hr = udp.join(listener);

	if (hr == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("join")), -1);
	}

	ssize_t sent = udp.send(message, ACE_OS_String::strlen(message) + 1);

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
	int hr = SendMulticast();
	return hr;
}
