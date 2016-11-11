
#include "WisIOSPushDao.h"


bool WisIOSPushDao::save(const std::string& device, int flag, int len, const std::string &msg)
{
	
	char sql[400] = {0};
	snprintf(sql,sizeof(sql),"INSERT INTO wis_ios_push_tbl(`submit_time`,`from_dev`,`flag`,`len`,`message`,`status`) VALUES(CURRENT_TIMESTAMP,'%s',%d,%d,'%s',1)",device.c_str(),flag,len,msg.c_str());
	
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	
	if(0 != access->ExecuteNonQuery(sql))
	{
		DbaModule_ReleaseNVDataAccess(access);
		return false;
	}
	DbaModule_ReleaseNVDataAccess(access);
    return true;
}