

#include "WisDeviceCommandDao.h"
#include "CNVDataAccess.h"
#include <sstream>


bool WisDeviceCommandDao::save(const std::string& user, const std::string& device, int cmd, int arglen, const std::string& arg )
{
	TRACE_IN();
    std::ostringstream SQL;
    SQL << "INSERT INTO wis_device_cmd_tbl(`user_id`,`dev_id`,`cmd`,`arg_len`,`arg`,`submit_time`,`status`) ";
    SQL << "VALUES("
        << "'" << user << "',"
        << "'" << device << "',"
        << cmd << ","
        << arglen << ","
        << "'" << arg << "',"
        << "CURRENT_TIMESTAMP,1)";
	
   CNVDataAccess * access =(CNVDataAccess *) DbaModule_GetNVDataAccess();
   if(NULL == access){
   		LOG_ERROR("get Access failed");
   	}

   if(-1 == access->ExecuteNonQuery(SQL.str().c_str())){
   		LOG_ERROR("save cmd failed sql = %s",SQL.str().c_str());
		DbaModule_ReleaseNVDataAccess(access);
		return false;
   	}
   DbaModule_ReleaseNVDataAccess(access);
   TRACE_OUT();
   return true;
}
