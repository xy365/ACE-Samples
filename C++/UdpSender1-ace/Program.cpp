//---------------------------------------------------------------------------------------------------
// UDP Sender: Unicast
//
// Sends UDP DGRAM in unicast mode
//
//---------------------------------------------------------------------------------------------------

//#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\Log_Msg.h>
//#include <ace\OS.h>
#include <ace/OS_NS_string.h>
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

int SendDatagram()
{
	const char* message = "this is a message!\n";

	ACE_INET_Addr  sender	(27016, ACE_LOCALHOST);
	ACE_INET_Addr  listener	(27015, ACE_LOCALHOST);
	ACE_SOCK_DGRAM udp		(sender);

	ssize_t sent = udp.send(message, ACE_OS_String::strlen(message) + 1, listener);

	char buffer[BUFSIZ];
	size_t size = sizeof(buffer);
	ssize_t received = udp.recv(buffer, size, listener);

	udp.close();

	if (sent == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("send")), -1);
	}

	cout << "sent:     " << sent << " bytes" << endl;
	cout << "received: " << received << " bytes" << endl;

	return 0;
}

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	int hr = SendDatagram();
	return hr;
}
