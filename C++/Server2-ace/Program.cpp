#pragma warning(disable:4996)

#include <ace\INET_Addr.h>
#include <ace\SOCK_Stream.h>
#include <ace\SOCK_Acceptor.h>
#include <ace\Time_Value.h>
#include <ace\Log_Msg.h>
#include <ace\os_include\os_netdb.h>
#include <string>

#pragma warning(default:4996)

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	ACE_INET_Addr		server(27015, ACE_LOCALHOST);
	//ACE_INET_Addr		portToListen("HAStatus");
	ACE_SOCK_Acceptor	acceptor;

	int hr = acceptor.open(server/*portToListen*/, 1);

	if (hr == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("acceptor.open")), 100);
	}

	while (1)
	{
		ACE_SOCK_Stream peer;
		ACE_INET_Addr	address;			// peer address
		ACE_Time_Value	timeout(ACE_Time_Value::max_time/*ACE_DEFAULT_TIMEOUT*/);

#if 0
		// Basic acceptor usage
		hr = acceptor.accept(peer);

		if (hr == -1)
		{
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) Failed to accept client connection\n")), 100);
		}
#else
		hr = acceptor.accept(peer, &address, &timeout, false);
#endif

		if (hr == -1)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Interrupted while waiting for connection\n")));
			}
			else if (ACE_OS::last_error() == ETIMEDOUT)
			{
				ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Timed out while waiting for connection\n")));
			}

			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) Failed to accept client connection\n")), 100);
		}

		ACE_TCHAR name[MAXHOSTNAMELEN];

		address.addr_to_string(name, MAXHOSTNAMELEN);

		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), name));

		char buffer[4096];
		ssize_t bytesReceived;

		while (bytesReceived = peer.recv(buffer, sizeof(buffer)) != -1)
		{
			//char* s = buffer;
			peer.send_n(buffer, bytesReceived);
			//peer.send_n(s, strlen(s));
		}

		peer.close();
		break;
	}

	return 0;
}
