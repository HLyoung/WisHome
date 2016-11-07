

#ifndef _DBNV_MANAGER_H_
#define _DBNV_MANAGER_H_

#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <time.h>
#include <sstream>
#include <assert.h>
#include "macro.h"
#include "libLog.h"
#include "CNVDataAccess.h"

class CCurrentTime
{
public:
	CCurrentTime();
	~CCurrentTime();
	static string GetCurrentTimeStringUTC();
	static string GetCurrentTimeStringLocal();
};


class CDatabaseConnection
{
public:
	enum {LIFE_COUNT_Default = 3};
public:
	CDatabaseConnection(CNVDataAccess *adpter);
	CDatabaseConnection(CNVDataAccess *apater,const UINT32 life_count);
	virtual ~CDatabaseConnection();

public:
	UINT64 GetKey();
	bool IsLock();
	void Activate();
	CNVDataAccess *GetAdapter() const{return adapter_;};
	bool IsLongConnection()const {return is_long_connection_;}
	void DecreaseLife();
	bool IsExpired();
	string GetCreateTime()const{return create_time_;}
	string GetLastActiveTime()const {return last_active_time_;}
	UINT32 GetLifeCount()const {return life_count_;}
	UINT32 GetReferenceCount()const {return reference_count_;}
	void Lock();
	void Unlock();
	
private:
	CNVDataAccess *adapter_;
	bool is_long_connection_;
	UINT32 life_count_;
	string create_time_;
	string last_active_time_;
	UINT32 reference_count_;
	std::mutex reference_count_lock_;
};



class CDbNVManager
{
public:
	CDbNVManager();
	virtual ~CDbNVManager();

	int SetDNS(const char *pcHost,const char *pcDbname, const char *pcUsername, const char *pcPwd);
	CNVDataAccess *GetNVDataAccess();
	void ReleaseNVDataAccess(CNVDataAccess * pNVDataAccess);
	static CDbNVManager* Instance();
	void Release();
private:
	static CDbNVManager* m_sInstance;
	CNVDataAccess *CreateNVDataAccess();
protected:
	std::mutex m_pConnectLock;
	std::map< UINT64, CDatabaseConnection*> m_mapConnection;
	string m_strHost;
	string m_strDbname;
	string m_strUsername;
	string m_strPwd;
};

#endif