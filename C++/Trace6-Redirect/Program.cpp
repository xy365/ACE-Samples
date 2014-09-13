#define ACE_NLOGGING 0

#pragma warning(disable:4996)

#include <ace\Log_Msg.h>
#include <ace\Null_Mutex.h>
#include <ace\Singleton.h>
#include "LogManager.h"

#pragma warning(default:4996)

#if _DEBUG
#pragma comment(lib, "aced")
#else
#pragma comment(lib, "ace")
#endif

using namespace Bisque;

void foo();

typedef ACE_Singleton<LogManager, ACE_Null_Mutex> logSingleton;

#define log logSingleton::instance()

// main
int ACE_TMAIN(int, ACE_TCHAR*[])
{
	log->RedirectToStderr();
	ACE_TRACE(ACE_TEXT("main"));

	log->RedirectToSyslog();
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IHi Mom\n")));

	foo();

	log->RedirectToDaemon();
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IGoodnight\n")));

	return 0;
}

void foo()
{
	ACE_TRACE(ACE_TEXT("foo"));

	log->RedirectToFile("log.txt");
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IHowdy Pardner\n")));
}
