/*
-- server.conf

dynamic Logger Service_Object * ACE:_make_ACE_Logging_Strategy() "-s foobar -f STDERR|OSTREAM|VERBOSE"
dynamic Server_Logging_Service Service_Object * netsvcs:_make_ACE_Server_Logging_Acceptor() active "-p 20009"

to start teh server:

$ACE_ROOT/netsvcs/servers/ace_netsvcs -f svc.conf

-- client.conf (not necessarily needed)

#dynamic Client_Logging_Service Service_Object * netsvcs:_make_ACE_Client_Logging_Acceptor() active "-p 20009 -h localhost"

#$ACE_ROOT/netsvcs/servers/ace_netsvcs -f client.conf

*/

#pragma warning(disable:4996)

#include <ace\Log_Msg.h>

#pragma warning(default:4996)

#pragma comment(lib, "ace")

int ACE_TMAIN(int, ACE_TCHAR* argv[])
{
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::LOGGER, ACE_DEFAULT_LOGGER_KEY);

	ACE_TRACE(ACE_TEXT("main"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IHi Mom\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IGoodnight\n")));

	return 0;
}
