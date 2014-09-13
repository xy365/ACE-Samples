#pragma warning(disable:4996)

#include <ace\Log_Msg.h>
#include <ace\streams.h>

#pragma warning(default:4996)

#pragma comment(lib, "ace")

using std::ostream;

int ACE_TMAIN(int, ACE_TCHAR* argv[])
{
	// Output to default destination (stderr)
	ACE_LOG_MSG->open(argv[0]);
	ACE_TRACE(ACE_TEXT("main"));
	ACE_OSTREAM_TYPE* output = new ofstream("ostream.output.test.txt");
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IThis will go to STDERR\n")));

	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IThis goes to STDERR & syslog\n")));

	ACE_LOG_MSG->msg_ostream(output, 0);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IThis will go to STDERR, ") 
						 ACE_TEXT("syslog & an ostream\n")));

	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::OSTREAM);

	delete output;

	return 0;
}
