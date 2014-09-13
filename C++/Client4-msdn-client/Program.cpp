// http://msdn.microsoft.com/en-us/library/windows/desktop/ms738545(v=vs.85).aspx

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::to_string;

int main(int argc, char** argv)
{
	// Validate parameters
	if (argc != 2)
	{
		cout << "usage: " << argv[0] << " <IPv4 address>" << endl;
		cout << "   inetaddr converts a string containing" << endl;
		cout << "   IPv4 address in one of the supported formats" << endl;
		cout << "   to unsigned long represented in IN_ADDR" << endl;
		cout << argv[0] << "      192.128.16.34" << endl;

		return E_FAIL;
	}

	// Initialize Winsock, version 2.2
	WSADATA	wsa;
	int		hr;

	hr = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (hr != 0)
	{
		cerr << "WSAStartup failed: " << to_string(hr) << endl;
		return E_FAIL;
	}

	//
	// 1. Declare an addrinfo object that contains a sockaddr structure and initialize these values. 
	//	  For this application, the Internet address family is unspecified so that either an IPv6 or IPv4 address can be returned. 
	//    The application requests the socket type to be a stream socket for the TCP protocol. 
	//

	struct addrinfo* result	= nullptr;
	struct addrinfo* ptr	= nullptr;
	struct addrinfo  hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;

	//
	// 2. Call the getaddrinfo function requesting the IP address for the server name passed on the command line. 
	//    The TCP port on the server that the client will connect to is defined by DEFAULT_PORT as 27015 in this sample. 
	//    The getaddrinfo function returns its value as an integer that is checked for errors. 
	//

	static const string defaultPort = "27015";

	hr = getaddrinfo(argv[1], defaultPort.c_str(), &hints, &result);

	if (hr != 0)
	{
		cerr << "getaddrinfo failed: " << to_string(hr) << endl;
		WSACleanup();
		return E_FAIL;
	}

	//
	// 3. Create a SOCKET object called ConnectSocket. 
	//

	SOCKET connectSocket = INVALID_SOCKET;

	//
	// 4. Call the socket function and return its value to the ConnectSocket variable. 
	//    For this application, use the first IP address returned by the call to getaddrinfo that matched the address family, 
	//    socket type, and protocol specified in the hints parameter. In this example, a TCP stream socket was specified 
	//    with a socket type of SOCK_STREAM and a protocol of IPPROTO_TCP. The address family was left unspecified (AF_UNSPEC), 
	//    so the returned IP address could be either an IPv6 or IPv4 address for the server. 
	//
	//    If the client application wants to connect using only IPv6 or IPv4, then the address family needs to be set to AF_INET6 
	//    for IPv6 or AF_INET for IPv4 in the hints parameter. 
	//

	ptr = result;

	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connectSocket == INVALID_SOCKET)
	{
		cerr << "Error at socket(): " << to_string(WSAGetLastError()) << endl;
		freeaddrinfo(result);
		WSACleanup();
		return E_FAIL;
	}

	//
	// 5. Connect to a socket.
	//    Call the connect function, passing the created socket and the sockaddr structure as parameters. Check for general errors.
	//
	//    The getaddrinfo function is used to determine the values in the sockaddr structure. In this example, 
	//    the first IP address returned by the getaddrinfo function is used to specify the sockaddr structure 
	//    passed to the connect. If the connect call fails to the first IP address, then try the next addrinfo 
	//    structure in the linked list returned from the getaddrinfo function.
	//   
	//    The information specified in the sockaddr structure includes:
	//       • the IP address of the server that the client will try to connect to.
	//       • the port number on the server that the client will connect to. This port was specified as port 27015 when the client called the getaddrinfo function.
	//

	hr = connect(connectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));

	if (hr == SOCKET_ERROR)
	{
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	// NOTE!
	// Should really try teh next address returned by getaddrinfo if the connect call failed.
	// For this simple example we just free resources returned by getaddrinfo and print an error message.

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		cerr << "Unable to connect to server!" << endl;
		WSACleanup();
		return E_FAIL;
	}

	// 6. Send and receive data.
	//    The send and recv functions both return an integer value of the number of bytes sent or received, 
	//    respectively, or an error. Each function also takes the same parameters: the active socket, 
	//    a char buffer, the number of bytes to send or receive, and any flags to use.
	//

	static const int bufferSize = 512;

	int  bytesSent;

	char* data = "this is a test.";

	// Send an initial buffer
	bytesSent = send(connectSocket, data, static_cast<int>(strnlen_s(data, bufferSize)), 0);

	if (bytesSent == SOCKET_ERROR)
	{
		cerr << "send failed: " << WSAGetLastError() << endl;
		closesocket(connectSocket);
		WSACleanup();
		return E_FAIL;
	}

	cout << "Bytes sent: " << bytesSent << endl;

	// Shutdown connection for sending since no more data will be sent.
	// The client still can use connectSocket to receive data.
	hr = shutdown(connectSocket, SD_SEND);

	if (hr == SOCKET_ERROR)
	{
		cerr << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(connectSocket);
		WSACleanup();
		return E_FAIL;
	}

	// Receive data until the server closes the connection
	int  tmpSize = bufferSize;
	int  bytesReceived;
	char buffer[bufferSize];

	do
	{
		bytesReceived = recv(connectSocket, buffer, tmpSize, 0);

		if (bytesReceived > 0)
		{
			cout << "Bytes received: " << bytesReceived << endl;
		}
		else if (bytesReceived == 0)
		{
			cout << "Connection closed." << endl;
		}
		else
		{
			cerr << "recv failed: " << WSAGetLastError() << endl;
		}
	}
	while (bytesReceived > 0);

	// 7. When the client is done sending data to the server, the shutdown function can be called specifying SD_SEND 
	//    to shutdown the sending side of the socket. This allows the server to release some of the resources for this socket. 
	//    The client application can still receive data on the socket. 

	hr = shutdown(connectSocket, SD_SEND);

	if (hr == SOCKET_ERROR)
	{
		cerr << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(connectSocket);
		WSACleanup();
		return E_FAIL;
	}


	//
	// Cleanup
	//

	closesocket(connectSocket);
	WSACleanup();

	return 0;
}
