#pragma warning(disable:4996)

#define ACE_NTRACE 0

#include <ace\streams.h>
#include <ace\Log_Msg.h>
#include <ace\Log_Msg_Callback.h>
#include <ace\Log_Record.h>
#include <ace\SString.h>

#pragma warning(default:4996)

#pragma comment(lib, "ace")

class Callback : public ACE_Log_Msg_Callback
{
public:
	void log(ACE_Log_Record& record)
	{
		record.print(ACE_TEXT(""), 0, cerr);
		record.print(ACE_TEXT(""), ACE_Log_Msg::VERBOSE, cerr);
	}
};

class Callback2 : public ACE_Log_Msg_Callback
{
public:
	void log(ACE_Log_Record& record)
	{
		cerr << "Log Message Received: " << endl;

		unsigned long		severity	= record.type();
		ACE_Log_Priority	priority	= static_cast<ACE_Log_Priority>(severity);
		const ACE_TCHAR*	name		= ACE_Log_Record::priority_name(priority);
		//const time_t		epoch		= record.time_stamp().sec();

		cerr << "\tType:   " << ACE_TEXT_ALWAYS_CHAR(name) << endl;
		cerr << "\tLength: " << record.length() << endl;
		//cerr << "\tTime:   " << ACE_TEXT_ALWAYS_CHAR(ACE_OS::ctime(&epoch)) << flush;
		cerr << "\tPid:    " << record.pid() << endl;

		ACE_CString data(">> ");
		data += ACE_TEXT_ALWAYS_CHAR(record.msg_data());

		cerr << "\tData:   " << data.c_str() << endl;
	}
};

int ACE_TMAIN(int, ACE_TCHAR*[])
{
	//auto c = std::unique_ptr<Callback>(new Callback);
	auto c = std::unique_ptr<Callback2>(new Callback2);

	ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);
	ACE_LOG_MSG->msg_callback(c.get());
	ACE_TRACE(ACE_TEXT("main"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IHi Mom\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IGoodnight\n")));

	return 0;
}

/*
-- Output with Callback:

Hi Mom
2013-04-16 11:14:26.183@@9960@LM_DEBUG@Hi Mom
Goodnight
2013-04-16 11:14:26.183@@9960@LM_INFO@Goodnight

-- Output with Callback2:

Log Message Received:
        Type:   LM_DEBUG
        Length: 48
        Pid:    9744
        Data:   >> Hi Mom

Log Message Received:
        Type:   LM_INFO
        Length: 56
        Pid:    9744
        Data:   >> Goodnight

-- Output with Callback2 and NTrace 0

Log Message Received:
        Type:   LM_TRACE
        Length: 96
        Pid:    10108
        Data:   >> (12036) calling main in file `Program.cpp' on line 57

Log Message Received:
        Type:   LM_DEBUG
        Length: 56
        Pid:    10108
        Data:   >>     Hi Mom

Log Message Received:
        Type:   LM_INFO
        Length: 56
        Pid:    10108
        Data:   >>     Goodnight

Log Message Received:
        Type:   LM_TRACE
        Length: 64
        Pid:    10108
        Data:   >> (12036) leaving main

*/
