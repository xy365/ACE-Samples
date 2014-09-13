#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <Windows.h>
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
	WSADATA			wsa;
	int				hr;
	unsigned long	address = INADDR_NONE;

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

	// Initialize Winsock
	hr = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (hr != 0)
	{
		cerr << "WSAStartup failed: " << to_string(hr) << endl;
		return E_FAIL;
	}

	// Call inet_addr(). If the call succeeds, the result variable will hold IN_ADDR
	address = inet_addr(argv[1]);

	if (address == INADDR_NONE)
	{
		cerr << "inet_addr failed and returned INADDR_NONE" << endl;
		WSACleanup();
		return E_FAIL;
	}

	if (address == INADDR_ANY)
	{
		cerr << "inet_addr failed and returned INADDR_ANY" << endl;
		WSACleanup();
		return E_FAIL;
	}

	cout << "inet_addr returned success" << endl;

	//
	// Here we could implement code to retreive each address and print out the hex bytes
	//

	for (int i = 0; i < 4; ++i)
	{
		char* ptr = reinterpret_cast<char*>(&address);

		cout << ptr[0] << endl;

		++ptr;
	}

	//
	// Cleanup
	//

	WSACleanup();

	return 0;
}
