


#include "WisBindDao.h"


void WisBindDao::sendBindResponse(BUS_ADDRESS_POINTER bus_address,int nCmd, int done )
{
    int result = done;
    GetUniteDataModuleInstance()->SendData(bus_address,nCmd,(char *)&result,sizeof(int),TCP_SERVER_MODE);
}


bool WisBindDao::addBind(BUS_ADDRESS_POINTER bus_address,const std::string& userUUID, const std::string& deviceUUID )
{
	TRACE_IN();
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	assert(NULL != access);

  	int  res = 0;
	bool ret = true;
	char sql[200] = {0};
	snprintf(sql,sizeof(sql),"SELECT * FROM wis_device_bind_tbl WHERE `dev_id`='%s' AND `user_id`='%s'",deviceUUID.c_str(),userUUID.c_str());
	int tmp = access->ExecuteNoThrow(sql);
	if(0 == tmp){
		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"INSERT INTO wis_device_bind_tbl(`dev_id`,`user_id`,`bind_time`) VALUES('%s','%s',CURRENT_TIMESTAMP)",deviceUUID.c_str(),userUUID.c_str());
		if(-1 != access->ExecuteNonQuery(sql)){
			res = 0;
			ret = true;
		}
		else{
			res = 1;
			ret = false;
		}
	}
	else if(0 < tmp){
		res = 2; 
		ret = true;
	}
	else {
		res = 1;
		ret = false;
	}
	DbaModule_ReleaseNVDataAccess((void*)access);
	sendBindResponse(bus_address,WIS_CMD_USER_BIND,res);
	WisLogDao::saveUserBindLog(userUUID,deviceUUID.length(),deviceUUID.c_str());
	LOG_INFO("ADD BIND :ret = %d, userId=%s, devId=%s",ret,userUUID.c_str(),deviceUUID.c_str());
	TRACE_OUT();
	return ret;
}

bool WisBindDao::delBind(BUS_ADDRESS_POINTER bus_address,const std::string& userUUID, const std::string& deviceUUID )
{
	TRACE_IN();
	char sql[200] = {0};
	snprintf(sql,sizeof(sql),"DELETE FROM wis_device_bind_tbl WHERE `dev_id`='%s' AND `user_id`='%s'",deviceUUID.c_str()\
			,userUUID.c_str());	
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	assert(NULL != access);
	
	if(-1 == access->ExecuteNonQuery(sql))
	{
		LOG_INFO("DELETE BIND FAILED: userId=%s,devId=%s",userUUID.c_str(),deviceUUID.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		sendBindResponse(bus_address,WIS_CMD_USER_UNBIND,1);
		return false;
	}
	sendBindResponse(bus_address,WIS_CMD_USER_UNBIND,0);
	DbaModule_ReleaseNVDataAccess(access);
	WisLogDao::saveUserUnBindLog(userUUID,deviceUUID.length(),deviceUUID.c_str());
	LOG_INFO("DELETE BIND SUCCESS: userId=%s,devId=%s",userUUID.c_str(),deviceUUID.c_str());
	TRACE_OUT();
	return true;
}

int  WisBindDao::getBindedDevices(const std::string& userUUID, std::map<std::string,WisDeviceInfo>& buddies )
{
	TRACE_IN();
	char sql[200] = {0};
	snprintf(sql,sizeof(sql),"SELECT `uuid`,`name`,`bind_time`,`status` FROM wis_device_tbl,wis_device_bind_tbl WHERE \
	`dev_id`=`uuid` AND `user_id`='%s'",userUUID.c_str());
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(access->ExecuteNoThrow(sql) < 1){
		DbaModule_ReleaseNVDataAccess(access);
		return 0;
		}
	while(access->FetchNewRow())
	{
		WisDeviceInfo buddy;
		memset( &buddy, 0, sizeof(WisDeviceInfo) );
        strncpy(buddy.uuid, access->GetString("uuid").c_str(), 32);
        strncpy(buddy.name, access->GetString("name").c_str(), 32);
        strncpy(buddy.time, access->GetString("bind_time").c_str(), 20);
        buddy.status = access->GetInt("status");
        buddies[buddy.uuid] = buddy;
	}
    DbaModule_ReleaseNVDataAccess(access);

	TRACE_OUT();
    return static_cast<int>(buddies.size());
}

int  WisBindDao::getBindedUsers(const std::string& devUUID, std::map<std::string,WisUserInfo>& buddies )
{

	TRACE_IN();	
	char sql[200] = {0};
	snprintf(sql,sizeof(sql),"SELECT `name`,`bind_time`,`status` FROM wis_user_tbl, \
	wis_device_bind_tbl WHERE `user_id`=`name` AND `dev_id`='%s'",devUUID.c_str());
   
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(access->ExecuteNoThrow(sql) < 1)
	{
		LOG_INFO("device(devUUID = %s) bind no user",devUUID.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		return 0;
	}
	while(access->FetchNewRow())
	{
		 WisUserInfo buddy;
         memset( &buddy, 0, sizeof(WisUserInfo) );
         strncpy(buddy.uuid, access->GetString("name").c_str(), 32);
         strncpy(buddy.time, access->GetString("bind_time").c_str(), 20);
         buddy.status = access->GetInt("status");
         buddies[buddy.uuid] = buddy;
	}
    DbaModule_ReleaseNVDataAccess(access);

	TRACE_OUT();
    return static_cast<int>(buddies.size());
}
