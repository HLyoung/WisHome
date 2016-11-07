


#include "libLog.h"
#include <iostream>
#include <string>

using namespace log4cxx;


CZxLogManager * CZxLogManager::instance = NULL;
bool CZxLogManager::m_bInitalized = false;
log4cxx::LoggerPtr CZxLogManager::loggerPtr = log4cxx::LoggerPtr();

CZxLogManager::CZxLogManager()
{
	
}
CZxLogManager::~CZxLogManager()
{
	if(NULL != instance)
	{
		delete(instance);
		instance = NULL;
	}	
	if(m_bInitalized)
	{
		DeInit();
	}
}

int CZxLogManager::Init(const char *configFile,const char *loggerName)
{
	PropertyConfigurator::configure(configFile);
	loggerPtr = log4cxx::Logger::getLogger(std::string(loggerName));
	m_bInitalized = true;
	return 0;
}

void CZxLogManager::DeInit()
{
	
	log4cxx::LogManager::shutdown();
	m_bInitalized = false;
}
int CZxLogManager::UseLog(const char *logName,bool bAdditivity)
{
	 
}

int CZxLogManager::LogTo(const char *logInfo,const char *logName,const LOG_LEVEL level,\
	const char *file,int line)
{
	if(!m_bInitalized)
		return -1;
	switch((int )level)
	{
		case LEVEL_DEBUG:
			LOG4CXX_DEBUG(loggerPtr,logInfo);
			break;
		case LEVEL_INFO:
			LOG4CXX_INFO(loggerPtr,logInfo);
			break;
		case LEVEL_WARN:
			LOG4CXX_WARN(loggerPtr,logInfo);
			break;
		case LEVEL_ERROR:
			LOG4CXX_ERROR(loggerPtr,logInfo);
			break;
		case LEVEL_FATAL:
			LOG4CXX_FATAL(loggerPtr,logInfo);
			break;
		default:
			break;
	}
	return 0;
}
int CZxLogManager::SetLevel(LOG_LEVEL level,const char *logName)
{
	
}

const LOG_LEVEL CZxLogManager::GetLevel(const char *logName)
{

}
CZxLogManager *GetCZxLogManagerInstance()
{
	return CZxLogManager::getInstance();
}

