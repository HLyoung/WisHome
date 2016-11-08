
#include "WisIOSTokenDao.h"
#include <string.h>


int WisIOSTokenDao::save(const std::string& uuid, const std::string& token )
{
	TRACE_IN();
    char sql[200] = {0};
	snprintf(sql,sizeof(sql),"update wis_ios_token_tbl2 set `token`='%s',`update_time`=CURRENT_TIMESTAMP where `user`='%s'",\
	token.c_str(),uuid.c_str());
	
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		return -1;
	}
	
	if(-1 == access->ExecuteNonQuery(sql)  || access->RowsAffected() == 0)
	{
		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"insert into wis_ios_token_tbl2 values('%s','%s',CURRENT_TIMESTAMP)",uuid.c_str(),\
		token.c_str());
		if(-1 == access->ExecuteNonQuery(sql))
		{
			LOG_ERROR("save token handle log failed. token='%s',uuid='%s'",token.c_str(),uuid.c_str());
			DbaModule_ReleaseNVDataAccess(access);
			return -1;			
		}		
	}
	DbaModule_ReleaseNVDataAccess(access);
	
	TRACE_OUT();
	return 0;
}

std::string WisIOSTokenDao::getToken(const std::string& uuid )
{
	TRACE_IN();
    char sql[100] = {0};
	snprintf(sql,sizeof(sql),"select `token` from wis_ios_token_tbl2 where `user`='%s'",uuid.c_str());

	CNVDataAccess* access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		return "";
	}
	if(access->ExecuteNoThrow(sql) < 1)
	{
		LOG_ERROR("get token failed uuid = %s",uuid.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		return "";
	}
	
	if(access->RowsAffected() == 1)
	{
		if(access->FetchNewRow())
		{
			
			std::string Res =  access->GetString("uuid");
			DbaModule_ReleaseNVDataAccess(access);
			return Res;
		}
	}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
    return "";
}

int WisIOSTokenDao::getTokens(std::vector<std::string>& uuids, std::set<std::string>& tokens )
{
	TRACE_IN();
    std::string selectSQL;
    selectSQL += "SELECT `token` FROM wis_ios_token_tbl2 WHERE `user` IN (";
    int index = 0;
    for ( size_t i = 0 ; i < uuids.size(); ++i ) {
        if( index++ != 0 ) {
            selectSQL += ",";
        }
        selectSQL += "'";
        selectSQL += uuids[i];
        selectSQL += "'";
    }
    selectSQL += ")";
    
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL == access)
	{
		LOG_ERROR("get database access failed");
		return 0;
	}
	if(access->ExecuteNoThrow(selectSQL.c_str()) <1)
	{
		LOG_ERROR("get tokens failed");
		DbaModule_ReleaseNVDataAccess(access);
		return 0;
	}
	if(access->RowsAffected() >= 1)
	{
		while(access->FetchNewRow())
		{
			tokens.insert(access->GetString("token"));
		}
	}
	DbaModule_ReleaseNVDataAccess(access);
	
	TRACE_OUT();
    return static_cast<int>(tokens.size());
}
