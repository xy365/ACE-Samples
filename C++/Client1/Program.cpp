#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\SOCK_Stream.h>
#include <ace\SOCK_Connector.h>
#include <ace\Log_Msg.h>

#pragma warning(default:4996)

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	ACE_INET_Addr		server(27015, ACE_LOCALHOST);
	ACE_SOCK_Connector	connectSocket;
	ACE_SOCK_Stream		peer;

	int hr = connectSocket.connect(peer, server);

	if (hr == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("connect")), 1);
	}

	// Send something
	peer.send_n("uptime\n", 7);

	static const int bufferSize = 512;
	char buffer[bufferSize];
	int  bytesReceived;

	bytesReceived = peer.recv(buffer, sizeof(buffer));

	write(1, buffer, bytesReceived);

	peer.close();

	return 0;
}
