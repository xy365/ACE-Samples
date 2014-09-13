#define ACE_NTRACE 0

#pragma warning(disable:4096)

#include <ace\Log_Msg.h>

#pragma comment(lib, "ace")

#pragma warning(default:4096)

void foo();

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_NOTICE, ACE_Log_Msg::PROCESS);
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IHi Mom\n")));

	foo();

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IGoodnight\n")));

	return 0;
}

void foo()
{
	ACE_TRACE(ACE_TEXT("foo"));
	ACE_DEBUG((LM_NOTICE, ACE_TEXT("%IHowdy Partner\n")));
}
