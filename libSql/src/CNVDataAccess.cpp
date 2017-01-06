


#include "CNVDataAccess.h"
#include <string.h>

CNVDataAccess::CNVDataAccess()
{
	if(NULL == mysql_init(&mysql))
	{
		LOG_ERROR("create sql object failed :%s ",mysql_error(&mysql));
	}
	isInited_ = true;
	pRes_ = NULL;
}

CNVDataAccess::~CNVDataAccess()
{
	mysql_close(&mysql);
}

void CNVDataAccess::Connect(const char *host,const char *pcDbname,const char *pcUsername,const char *pcPwd)
{
	TRACE_IN();
	
	if(!IsInited())
		LOG_ERROR("sql ist not inited");
	if(NULL == mysql_real_connect(&mysql,
								  host,						  
								  pcUsername,
								  pcPwd,
								  pcDbname,
								  0,
								  NULL,
								  0
								 ))
	{
		LOG_ERROR("connect to DB failed: host=\"%s\",DBname=\"%s\",username=\"%s\",passwd=\"%s\"",host,pcDbname,pcUsername,pcPwd);
		return;
	}

	int value = 1;
	mysql_options(&mysql, MYSQL_OPT_RECONNECT, (char*)&value); 
	
	TRACE_OUT();
}
void CNVDataAccess::Disconnect()
{
	mysql_close(&mysql);
}

//ִ��û�н�������ص���䣬eg:update,insert,delete
int CNVDataAccess::ExecuteNonQuery(const char *pcCmdText)
{
	if(IsInited())
		if(!mysql_real_query(&mysql,pcCmdText,strlen(pcCmdText)))
			return 0;
	LOG_INFO("EXECUTE NON QUERY FAILED: %s",pcCmdText);
	return -1;
}
void CNVDataAccess::ExecuteDataSet(const char *pcCmdText)
{
	
	if(!IsInited())
		LOG_ERROR("sql is not inited");
	if(0 != mysql_real_query(&mysql,pcCmdText,strlen(pcCmdText)))
		LOG_ERROR("query cmd failed cmd=%s",pcCmdText);
}


bool CNVDataAccess::IsAlive()const 
{
	return true;
}

bool CNVDataAccess::IsTableExist(const char *pcTablename)
{
	return true;
}
void CNVDataAccess::SetAutoCommit(bool atuo)
{
	
}

bool CNVDataAccess::GetAutoCommit()const
{
	return true;
	
}

void CNVDataAccess::Commit(bool bRebackAutoCommit)
{
	
}

void CNVDataAccess::Rollback()
{
	
}

string &CNVDataAccess::SetOption(const char *pcOptionName)
{
	string what;
	return what;
}


//ִ���н�������ص�sql��� eg:select
int CNVDataAccess::ExecuteNoThrow(const char *pcCmdText)
{
	if(!mysql_real_query(&mysql,pcCmdText,strlen(pcCmdText)))
		if(IsResultSet())
			return mysql_affected_rows(&mysql);
	LOG_INFO("EXECUTE NON THROW FAILED: %s",pcCmdText);
	return -1;
}

bool CNVDataAccess::IsResultSet()
{
	pRes_ = mysql_store_result(&mysql);
	if(NULL != pRes_)
		return true;
	return false;
}

long CNVDataAccess::RowsAffected()
{
		return mysql_affected_rows(&mysql);
}

bool CNVDataAccess::FetchFirst()
{
	if(NULL != this->pRes_){
		row_ = mysql_fetch_row(pRes_);
		if(NULL != row_)
			return true;
		}
	return false;
}

bool CNVDataAccess::FetchNext()
{
	row_ = mysql_fetch_row(pRes_);
	if(NULL == row_){
		mysql_free_result(pRes_);
		return false;
		}
	return true;
}

bool CNVDataAccess::FetchNewRow()
{
	if(pRes_ != NULL){
		row_ = mysql_fetch_row(pRes_);
		if(NULL != row_){
			mysql_free_result(pRes_);		
			pRes_ = NULL;
			return true;
			}
		}
	return false;
}

bool CNVDataAccess::NextResultSet()
{
	return true;
}

int CNVDataAccess::GetFieldCount()
{
	return mysql_num_fields(pRes_);
}

MYSQL_FIELD * CNVDataAccess::Field(int nField)
{
	MYSQL_FIELD *filed;	
	if(nField > mysql_field_count(&mysql)-1)
		return NULL;
	filed =  mysql_fetch_field_direct(pRes_,nField);
	if(NULL == filed)
		return NULL;
	return filed;
}

MYSQL_FIELD *CNVDataAccess::Field(const char *pcField)
{
	MYSQL_FIELD *field;
	for(int pos = 0;pos < mysql_num_fields(pRes_)-1;pos++){
		field = mysql_fetch_field_direct(pRes_,pos);
		if(!strncmp(pcField,field->name,strlen(pcField)))
			return field;
		}
	return NULL;
}

string & CNVDataAccess::Param(const char *pcParamByName)
{
	string wait;
	return wait;
}

bool CNVDataAccess::BulkCopy(const char *pcDestTableName,const \
char *pcDtaFileName)
{
	return true;
}

void CNVDataAccess::GetRunningFunctions(vector<string> &result)
{
	
}

long CNVDataAccess::FieldsOfRow()
{
	return mysql_num_fields(pRes_);
}

void CNVDataAccess::RegisterCallBack()
{
	
}

std::string CNVDataAccess::GetString(std::string lineName)
{
	std::string strRes;
	int nFields = mysql_num_fields(pRes_);
	MYSQL_FIELD *fields = mysql_fetch_fields(pRes_);
	
	for(int i=0;i<nFields;i++){
		if(lineName == std::string(fields[i].name)){
			strRes = row_[i];
			break;
			}
		}
	return strRes;
}

int CNVDataAccess::GetInt(std::string lineName)
{
	int nFields = mysql_num_fields(pRes_);
	MYSQL_FIELD *fields = mysql_fetch_fields(pRes_);
	
	for(int i= 0;i<nFields;i++)
		if(lineName == std::string(fields[i].name))
			return atoi(row_[i]);
}

void CNVDataAccess::FreeResult(void)
{
	if(NULL != pRes_){
		mysql_free_result(pRes_);
		pRes_ = NULL;
		}
}

long  CNVDataAccess::ResNumRows()
{
	if(NULL != pRes_)
		return mysql_num_rows(pRes_);
	else
		return -1;
}