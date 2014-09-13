// Getting started:
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms738545(v=vs.85).aspx

// Complete code:
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms737593(v=vs.85).aspx

//
// For historical reasons, the Windows.h header defaults to including the Winsock.h header file for Windows Sockets 1.1. 
// The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file 
// required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. 
//

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
		cerr << "WSAStartup failed: " << hr << endl;
		return E_FAIL;
	}

	//
	// 1. The getaddrinfo function is used to determine the values in the sockaddr structure:
	//    ◦ AF_INET is used to specify the IPv4 address family.
	//    ◦ SOCK_STREAM is used to specify a stream socket.
	//    ◦ IPPROTO_TCP is used to specify the TCP protocol .
	//    ◦ AI_PASSIVE flag indicates the caller intends to use the returned socket address structure 
	//      in a call to the bind function. When the AI_PASSIVE flag is set and nodename parameter 
	//      to the getaddrinfo function is a NULL pointer, the IP address portion of the socket address structure 
	//      is set to INADDR_ANY for IPv4 addresses or IN6ADDR_ANY_INIT for IPv6 addresses.
	//    ◦ 27015 is the port number associated with the server that the client will connect to.
	//
	//    The addrinfo structure is used by the getaddrinfo function.
	//

	static const string defaultPort = "27015";

	struct addrinfo* result = nullptr;
	struct addrinfo* ptr	= nullptr;
	struct addrinfo  hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;
	hints.ai_flags		= AI_PASSIVE;

	// Resolve local address and port to be used by the server
	hr = getaddrinfo(nullptr, defaultPort.c_str(), &hints, &result);

	if (hr != 0)
	{
		cerr << "getaddrinfo failed: " << hr << endl;
		WSACleanup();
		return E_FAIL;
	}

	//
	// 2. Create a SOCKET object called ListenSocket for the server to listen for client connections. 
	//

	SOCKET listenSocket = INVALID_SOCKET;

	//
	// 3. Call the socket function and return its value to the ListenSocket variable. For this server application, 
	//    use the first IP address returned by the call to getaddrinfo that matched the address family, socket type, 
	//    and protocol specified in the hints parameter. In this example, a TCP stream socket for IPv4 was requested 
	//    with an address family of IPv4, a socket type of SOCK_STREAM and a protocol of IPPROTO_TCP. 
	//    So an IPv4 address is requested for the ListenSocket. 
	//
	//    If the server application wants to listen on IPv6, then the address family needs to be set to AF_INET6 
	//    in the hints parameter. If a server wants to listen on both IPv6 and IPv4, two listen sockets must be created, 
	//    one for IPv6 and one for IPv4. These two sockets must be handled separately by the application.
	//
	//    Windows Vista and later offer the ability to create a single IPv6 socket that is put in dual stack mode 
	//    to listen on both IPv6 and IPv4. For more information on this feature, see Dual-Stack Sockets.
	//

	listenSocket = socket(result ->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return E_FAIL;
	}

	//
	// 4. The sockaddr structure holds information regarding the address family, IP address, and port number. 
	//    Call the bind function, passing the created socket and sockaddr structure returned 
	//    from the getaddrinfo function as parameters. Check for general errors.
	//

	hr = bind(listenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));

	if (hr == SOCKET_ERROR)
	{
		cerr << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return E_FAIL;
	}

	// Once the bind function is called, the address information returned by the getaddrinfo function is no longer needed. 
	// The freeaddrinfo function is called to free the memory allocated by the getaddrinfo function for this address information.

	freeaddrinfo(result);

	//
	// 5. Call the listen function, passing as parameters the created socket and a value for the backlog, 
	//    maximum length of the queue of pending connections to accept. In this example, the backlog parameter 
	//    was set to SOMAXCONN. This value is a special constant that instructs the Winsock provider for this socket 
	//    to allow a maximum reasonable number of pending connections in the queue. Check the return value for general errors.

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return E_FAIL;
	}

	// 6. Create a temporary SOCKET object called ClientSocket for accepting connections from clients. 

	SOCKET clientSocket;

	// 7. Normally a server application would be designed to listen for connections from multiple clients. 
	//    For high-performance servers, multiple threads are commonly used to handle the multiple client connections. 
	//
	//    There are several different programming techniques using Winsock that can be used to listen for multiple client connections. 
	//    One programming technique is to create a continuous loop that checks for connection requests using the listen function 
	//    (see Listening on a Socket). If a connection request occurs, the application calls the accept, AcceptEx, or WSAAccept function 
	//    and passes the work to another thread to handle the request. Several other programming techniques are possible.
	//
	//    Note that this basic example is very simple and does not use multiple threads. The example also just listens for and accepts only a single connection. 
	//
	//    . . .
	//
	//    When the client connection has been accepted, a server application would normally pass the accepted client socket 
	//    (the ClientSocket variable) to a worker thread or an I/O completion port and continue accepting additional connections. 
	//    In this basic example, the server continues to the next step.
	//
	//    There are a number of other programming techniques that can be used to listen for and accept multiple connections. 
	//    These include using the select or WSAPoll functions. Examples of some of these various programming techniques are illustrated 
	//    in the Advanced Winsock Samples included with the Microsoft Windows Software Development Kit (SDK).
	//
	//    NOTE  
	//    On Unix systems, a common programming technique for servers was for an application to listen for connections. 
	//    When a connection was accepted, the parent process would call the fork function to create a new child process to handle the client connection, 
	//    inheriting the socket from the parent. This programming technique is not supported on Windows, since the fork function is not supported. 
	//    This technique is also not usually suitable for high-performance servers, since the resources needed to create a new process 
	//    are much greater than those needed for a thread. 
	//

	cout << "Ready to receive data..." << endl << endl;

	clientSocket = INVALID_SOCKET;

	clientSocket = accept(listenSocket, nullptr, nullptr);

	cout << "Client connected" << endl << endl;

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "accept failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return E_FAIL;
	}

	//
	// 8. Receive and send data on a socket
	//
	//    The send and recv functions both return an integer value of the number of bytes sent or received, respectively, or an error. 
	//    Each function also takes the same parameters: the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.
	//

	static const int bufferSize = 512;			// buffer length

	char buffer[bufferSize];					// receive buffer
	int  received;
	int  tmpSize = bufferSize;
	int	 sendResult;

	// Receive until the peer shuts down the connection
	do
	{
		received = recv(clientSocket, buffer, tmpSize, 0);

		if (received > 0)
		{
			cout << "Bytes received: " << received << endl;

			// Echo teh buffer back to the sender
			sendResult = send(clientSocket, buffer, received, 0);

			if (sendResult == SOCKET_ERROR)
			{
				cerr << "send failed: " << WSAGetLastError() << endl;
				closesocket(clientSocket);
				WSACleanup();
				return E_FAIL;
			}

			cout << "Bytes sent: " << sendResult << endl;
		}
		else if (received == 0)
		{
			cout << "Connection closing..." << endl;
		}
		else
		{
			cout << "recv failed: " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return E_FAIL;
		}
	} 
	while (received > 0);

	cout << "Received data, shutting down" << endl << endl;

	//
	// 9. When the server is done sending data to the client, the shutdown function can be called specifying SD_SEND 
	//    to shutdown the sending side of the socket. This allows the client to release some of the resources for this socket. 
	//    The server application can still receive data on the socket. 
	//

	hr = shutdown(clientSocket, SD_SEND);

	if (hr == SOCKET_ERROR)
	{
		cout << "shutdown failed: " << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return E_FAIL;
	}

	//
	// Cleanup
	//

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
