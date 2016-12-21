

#ifndef _LIBLOG4CXX_H_
#define _LIBLOG4CXX_H_

#include <mutex>
#include <stdio.h>

#include <log4cxx/helpers/objectptr.h>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/level.h>
#include <log4cxx/simplelayout.h>

enum LOG_LEVEL
{
	LEVEL_DEBUG = 10000,
	LEVEL_INFO  = 20000,
	LEVEL_WARN  = 30000,
	LEVEL_ERROR = 40000,
	LEVEL_FATAL = 50000,
};

#define LOG_MAX_LEN 4096*4

class CZxLogManager
{
public:
	
    virtual ~CZxLogManager();
	CZxLogManager();
	static CZxLogManager* getInstance()
	{
		if(instance == NULL)
		{
			instance = new CZxLogManager;
		}
		return instance;
	}
	static int Init(const char *configFile ="log.properties",\
	const char *loggerName="fa");
	static void DeInit();
	static int UseLog(const char *logName = 0,bool bAdditivity = false);
	static int LogTo(const char *logInfo,const char *logName,const LOG_LEVEL level \
					,const char* file = 0,int line = -1);
	static int SetLevel(LOG_LEVEL level,const char *logName = 0);
	static const LOG_LEVEL GetLevel(const char *logName = 0);
	
private:
  
	static CZxLogManager* instance;
	static log4cxx::LoggerPtr loggerPtr;
	static bool m_bInitalized;
};

extern "C" CZxLogManager *GetCZxLogManagerInstance();

#ifdef DEBUG
#define LOG_DEBUG(format,...) \
do{\
	char str[LOG_MAX_LEN] = {0};\
	sprintf(str,"[%s@%s,%d]:" format"\n",\
		__func__,__FILE__,(int )__LINE__,##__VA_ARGS__);\
	CZxLogManager::getInstance()->LogTo(str,NULL,LEVEL_DEBUG,\
										NULL);\
}while(0)
	
#define LOG_INFO(format,...) \
do{\
    char str[LOG_MAX_LEN] = {0};\
	sprintf(str,"[%s@%s,%d]:" format"\n",\
		__func__,__FILE__,(int )__LINE__,##__VA_ARGS__);\
	CZxLogManager::getInstance()->LogTo(str,NULL,LEVEL_INFO,\
										NULL);\
}while(0)

#define LOG_WARN(format,...) \
do{\
	char str[LOG_MAX_LEN] = {0};\
	sprintf(str,"[%s@%s,%d]:" format"\n",\
		__func__,__FILE__,(int )__LINE__,##__VA_ARGS__);\
	CZxLogManager::getInstance()->LogTo(str,NULL,LEVEL_WARN,\
										NULL);\
}while(0)

#define LOG_ERROR(format,...) \
do{\
	char str[LOG_MAX_LEN] = {0};\
	sprintf(str,"[%s@%s,%d]:" format"\n",\
	__func__,__FILE__,(int )__LINE__,##__VA_ARGS__);\
	CZxLogManager::getInstance()->LogTo(str,NULL,LEVEL_ERROR,\
										NULL);\
}while(0)

#define LOG_FATAL(format,...) \
do{\
	char str[LOG_MAX_LEN] = {0};\
	sprintf(str,"[%s@%s,%d]:" format"\n",\
		__func__,__FILE__,(int )__LINE__,##__VA_ARGS__);\
	CZxLogManager::getInstance()->LogTo(str,NULL,LEVEL_FATAL,\
										NULL);\
}while(0)

#define TRACE_IN() \
do{\
	LOG_DEBUG("trace in %s",__func__);\
}while(0)
	
#define TRACE_OUT()\
do{\
	LOG_DEBUG("trace out %s",__func__);\
}while(0)
#else   //DEBUG
#define LOG_INFO(format,...)
#define LOG_WARN(format,...)
#define LOG_ERROR(format,...)
#define LOG_DEBUG(format,...)
#define LOG_FATAL(format,...)
#define TRACE_IN()
#define TRACE_OUT()
#endif   //DEBUG 


#endif
