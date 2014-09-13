#pragma once

#pragma warning(disable:4996)

#include <ace\Log_Msg.h>

#pragma warning(default:4996)

namespace Bisque
{
	class LogManager
	{
	public:
		LogManager(void);
		~LogManager(void);

		void RedirectToDaemon	(const ACE_TCHAR* name = ACE_TEXT(""));
		void RedirectToSyslog	(const ACE_TCHAR* name = ACE_TEXT(""));
		void RedirectToOStream	(ACE_OSTREAM_TYPE* output);
		void RedirectToFile		(const char* name);
		void RedirectToStderr	();

		ACE_Log_Msg_Callback* RedirectToCallback(ACE_Log_Msg_Callback* callback);
	};
}
