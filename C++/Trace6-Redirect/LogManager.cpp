#include "LogManager.h"
#include <iostream>
#include <fstream>

using namespace Bisque;

LogManager::LogManager(void)
{
}

LogManager::~LogManager(void)
{
}

void LogManager::RedirectToDaemon(const ACE_TCHAR* name)
{
	ACE_LOG_MSG->open(name, ACE_Log_Msg::LOGGER, ACE_DEFAULT_LOGGER_KEY);
}

void LogManager::RedirectToSyslog(const ACE_TCHAR* name)
{
	ACE_LOG_MSG->open(name, ACE_Log_Msg::SYSLOG, name);
}

void LogManager::RedirectToOStream(ACE_OSTREAM_TYPE* output)
{
	ACE_LOG_MSG->msg_ostream(output);
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
}

void LogManager::RedirectToFile(const char* name)
{
	using std::ofstream;

	ofstream file(name, ios::out | ios::app);
	if (file.is_open())
	{
		this->RedirectToOStream(&file);
		file.close();
	}
}

void LogManager::RedirectToStderr()
{
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::OSTREAM | ACE_Log_Msg::LOGGER);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR);
}

ACE_Log_Msg_Callback* LogManager::RedirectToCallback(ACE_Log_Msg_Callback* callback)
{
	auto previous = ACE_LOG_MSG->msg_callback(callback);

	if (previous == nullptr)
	{
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::MSG_CALLBACK);
	}
	else
	{
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);
	}

	return previous;
}
