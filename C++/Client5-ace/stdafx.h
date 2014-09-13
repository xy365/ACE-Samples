#pragma once

#include <ace\Connector.h>
#include <ace\INET_Addr.h>
#include <ace\Reactor.h>
#include <ace\Reactor_Notification_Strategy.h>
#include <ace\SOCK_Connector.h>
#include <ace\SOCK_Stream.h>
#include <ace\Svc_Handler.h>

// lib
#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif