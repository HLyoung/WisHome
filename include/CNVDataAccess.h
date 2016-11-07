

#ifndef _CNV_DATA_ACCES_H_
#define _CNV_DATA_ACCES_H_

extern "C"{
#include <mysql/mysql.h>
#include <mysql/mysql_com.h>
}
#include <iostream>
#include <vector>
#include <assert.h>
#include "libLog.h"
#include "macro.h"


using namespace std;
class CNVDataAccess
{
public:
	CNVDataAccess();
	virtual ~CNVDataAccess();
	virtual void Connect(const char *host,const char *pcDbname,const char *pcUsername,const \
						 char *pcPwd);
	virtual void Disconnect(void);
	virtual bool IsAlive()const;
	virtual bool IsTableExist(const char *pcTableName);
	virtual void SetAutoCommit(bool atuo);
	virtual bool GetAutoCommit(void) const;
	virtual void Commit(bool bRebackAutoCommit);
	virtual void Rollback();
	virtual string &SetOption(const char*pcOptionName);
	virtual int  ExecuteNonQuery(const char *pcCmdText);
	virtual void ExecuteDataSet(const char *pcCmdText);
	virtual int ExecuteNoThrow(const char*pcCmdText);
	virtual bool IsResultSet(void);
	virtual long RowsAffected(void);
	virtual long FieldsOfRow(void);
	virtual bool FetchFirst(void);
	virtual bool FetchNext(void);
	
	virtual bool FetchNewRow(void);
	virtual bool NextResultSet(void);
	virtual int GetFieldCount(void);
	virtual MYSQL_FIELD *Field(int nField);
	virtual MYSQL_FIELD *Field(const char *pcField);
	virtual string &Param(const char *pcParamByName);
	virtual bool BulkCopy(const char *pcDescTableName,const char* pcDataFileName);
	virtual void GetRunningFunctions(vector<string> &result);
	virtual bool IsInited()const {return isInited_;}
	virtual std::string  GetString(std::string lineName);
	virtual int GetInt(std::string lineName);
	virtual void RegisterCallBack();
	virtual void FreeResult(void);
	virtual long ResNumRows();
private:
    bool isInited_;
	MYSQL mysql;
	MYSQL_RES *pRes_;  //当前sql语句返回的结果集
	MYSQL_ROW row_;  //当前选定的行。
};






#endif