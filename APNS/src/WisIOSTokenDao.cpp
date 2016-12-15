
#include "WisIOSTokenDao.h"
#include <string.h>


int WisIOSTokenDao::save(const std::string& uuid, const std::string& token )
{
	TRACE_IN();
    char sql[200] = {0};
	snprintf(sql,sizeof(sql),"update wis_ios_token_tbl2 set `user`='%s',`update_time`=CURRENT_TIMESTAMP where `token`='%s'",uuid.c_str(),token.c_str());
	
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(-1 != access->ExecuteNonQuery(sql)){
			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql),"insert into wis_ios_token_tbl2 values('%s','%s',CURRENT_TIMESTAMP)",uuid.c_str(),	token.c_str());
			if(-1 != access->ExecuteNonQuery(sql))
				return 0;	
			}}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
	return -1;
}

int  WisIOSTokenDao::getToken(const std::string& uuid,std::set<std::string> &tokens )
{
	TRACE_IN();
    char sql[100] = {0};
	snprintf(sql,sizeof(sql),"select `token` from wis_ios_token_tbl2 where `user`='%s'",uuid.c_str());

	CNVDataAccess* access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		if(access->ExecuteNoThrow(sql) >= 1){
			while(access->FetchNewRow()){
				std::string token = access->GetString("token");
				if(token.length() != 0)
					tokens.insert(token);
				}}}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
    return static_cast<int>(tokens.size());
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
	if(NULL != access){
		if(access->ExecuteNoThrow(selectSQL.c_str()) >=1){	
					while(access->FetchNewRow()){
						std::string token = access->GetString("token");
						if(token.length() != 0)
							tokens.insert(token);
						}}}
	DbaModule_ReleaseNVDataAccess(access);
	TRACE_OUT();
	return static_cast<int>(tokens.size());
}
