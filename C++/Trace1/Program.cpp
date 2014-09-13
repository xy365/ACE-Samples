#pragma warning(disable:4996)				// ACE is using deprecated functions with security vulnerabilities like strcat, etc.

#include <ace\Log_Msg.h>

#pragma comment(lib, "ace")

#pragma warning(default:4996)

void foo();

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IHi Mom\n")));

	foo();

	ACE_DEBUG((LM_INFO, ACE_TEXT("%IGoodnight\n")));

	return 0;
}

void foo()
{
	ACE_TRACE(ACE_TEXT("foo"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IHowdy Partner\n")));
}
