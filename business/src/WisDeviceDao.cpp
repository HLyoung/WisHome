
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
	
	TRACE_IN();
    char insertSQL[1024] = {0};
    snprintf(insertSQL, sizeof(insertSQL),
        "INSERT INTO wis_device_tbl VALUES('%s','device','1','1',CURRENT_TIMESTAMP,CURRENT_TIMESTAMP,1,1,'%s')",
             uuid.c_str(), name.c_str());
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		 DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	if( -1 ==  access->ExecuteNonQuery(insertSQL))
	{
		LOG_ERROR("execute sql(%s) query failed",insertSQL);
		 DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	 DbaModule_ReleaseNVDataAccess(access);
	
	TRACE_OUT();
	return true;
}
bool WisDeviceDao::login(const std::string& uuid ,const std::string &name )
{
	
	TRACE_IN();
    char updateSQL[1024] = {0};
	int rows = 0;
    snprintf( updateSQL, sizeof(updateSQL),
        "UPDATE wis_device_tbl SET `status`=1,`login_cnt`=`login_cnt`+1,`name`='%s'  WHERE `uuid`='%s'",name.c_str(),uuid.c_str()
    );
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		 DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	
	if( -1 == (rows = access->ExecuteNonQuery(updateSQL)))
	{
		LOG_ERROR("execute sql(%s) query failed",updateSQL);
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	
	if(rows == 1 || rows == 0)
	{
	 	DbaModule_ReleaseNVDataAccess(access);
		return true;
	}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
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
			return true;
		   }
		}
	DbaModule_ReleaseNVDataAccess(access);
    return false;
}

bool WisDeviceDao::logoutAll( )
{
	TRACE_IN();
    static const std::string SQL = "UPDATE wis_device_tbl SET `status`=0";
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	
	if(-1 == access->ExecuteNonQuery(SQL.c_str()))
	{
		LOG_ERROR("execute sql(%s) query failed",SQL.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
    return true;
}

bool WisDeviceDao::getDevice( const std::string& uuid, WisDeviceInfo& info )
{
	TRACE_IN();
    std::string SQL = "SELECT * FROM wis_device_tbl where `uuid`='";SQL += uuid; SQL += "'";
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	
	if(access->ExecuteNoThrow(SQL.c_str()) < 1)
	{
		LOG_ERROR("execute sql(%s) query failed",SQL.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
    if(access->RowsAffected() == 1) {
        if( access->FetchNewRow()) {
            strncpy( info.uuid, access->GetString("uuid").c_str(), 32);
            strncpy( info.name, access->GetString("name").c_str(), 32);
            info.status = access->GetInt("status");
			DbaModule_ReleaseNVDataAccess(access);
            return true;
        }
    }
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
    return false;
}
