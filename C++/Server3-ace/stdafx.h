#pragma once

#pragma warning(disable:4996)

#include <ace\Auto_Ptr.h>
#include <ace\INET_Addr.h>
#include <ace\Log_Msg.h>
#include <ace\Acceptor.h>
#include <ace\Reactor.h>
#include <ace\SOCK_Acceptor.h>
#include <ace\SOCK_Stream.h>
#include <ace\Time_Value.h>
#include <ace\os_include\os_netdb.h>

#pragma warning(default:4996)

#include <iostream>

// Libraries
#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif
