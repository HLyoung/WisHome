
#include "WisDeviceDao.h"


bool WisDeviceDao::check(const std::string& uuid )
{
	TRACE_IN();
	char sql[200] = {0};
	snprintf(sql,sizeof(sql),"SELECT * FROM wis_device_tbl WHERE uuid='%s'",uuid.c_str());

	bool ret = false;
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(access->ExecuteNoThrow(sql) >= 1)
			ret = true;
		DbaModule_ReleaseNVDataAccess(access);
		}
	TRACE_OUT();
	return ret;
}

bool WisDeviceDao::regist(const std::string& uuid, const std::string& name)
{
    char insertSQL[1024] = {0};
    snprintf(insertSQL, sizeof(insertSQL),"INSERT INTO wis_device_tbl VALUES('%s','device','1','1',CURRENT_TIMESTAMP,CURRENT_TIMESTAMP,1,1,'%s')",uuid.c_str(), name.c_str());
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if( -1 !=  access->ExecuteNonQuery(insertSQL)){
			LOG_INFO("DEVICE REGIST SUCCESS: devID=%s,name=%s",uuid.c_str(),name.c_str());
			return true;
			}
		 DbaModule_ReleaseNVDataAccess(access);
		}
	LOG_INFO("DEVICE REGIST FAILED: devID=%s,name=%s",uuid.c_str(),name.c_str());
	return false;
}
bool WisDeviceDao::login(const std::string& uuid ,const std::string &name )
{
    char updateSQL[1024] = {0};
    snprintf( updateSQL, sizeof(updateSQL), "UPDATE wis_device_tbl SET `status`=1,`login_cnt`=`login_cnt`+1,`name`='%s',`login_time`=CURRENT_TIMESTAMP  WHERE `uuid`='%s'",name.c_str(),uuid.c_str());
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(-1 != access->ExecuteNonQuery(updateSQL)){
		 	DbaModule_ReleaseNVDataAccess(access);
			LOG_INFO("DEVICE LOGIN SUCCESS: devID=%s,name=%s",uuid.c_str(),name.c_str());
			return true;
		}
		DbaModule_ReleaseNVDataAccess(access);
		}
	LOG_INFO("DEVICE LOGIN FAILED: devID=%s,name=%s",uuid.c_str(),name.c_str());
    return false;
}

bool WisDeviceDao::logout(const std::string& uuid )
{
    char updateSQL[200] = {0};
    snprintf( updateSQL, sizeof(updateSQL),"UPDATE wis_device_tbl SET `status`=0 WHERE `uuid`='%s'",uuid.c_str()); 
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(-1 != access->ExecuteNonQuery(updateSQL)){
			DbaModule_ReleaseNVDataAccess(access);
			LOG_INFO("DEVICE LOGOUT SUCCESS: devID=%s",uuid.c_str());
			return true;
		   }
		DbaModule_ReleaseNVDataAccess(access);
		}
	LOG_INFO("DEVICE LOGOUT FAILED: devID=%s",uuid.c_str());
    return false;
}

bool WisDeviceDao::logoutAll( )
{
    static const std::string SQL = "UPDATE wis_device_tbl SET `status`=0";
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(-1 != access->ExecuteNonQuery(SQL.c_str())){
			DbaModule_ReleaseNVDataAccess(access);
			LOG_INFO("ALL DEVICE LOGOUT SUCCESS");
			return true;
		}
		DbaModule_ReleaseNVDataAccess(access);
	}
    LOG_INFO("ALL DEVICE LOGOUT FAILED");
    return false;
}
