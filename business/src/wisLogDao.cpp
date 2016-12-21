
#include "wisLogDao.h"


void WisLogDao::saveLog(int src, int logType, const std::string& srcId, int argLen, const char* arg)
{
	TRACE_IN(); 
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
	    char sql[2000] = {0};
	    snprintf(sql, sizeof(sql),\
				  "insert into wis_log_tbl(`submit_time`,`src`,`type`,`src_id`,`arg_len`,`arg`) values (CURRENT_TIMESTAMP,%d,%d,'%s',%d,'%s')",\
	              src, logType, srcId.c_str(), argLen, arg);
	   access->ExecuteNonQuery(sql);
	   DbaModule_ReleaseNVDataAccess(access);
	   }	
	TRACE_OUT();
}
