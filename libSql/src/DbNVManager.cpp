

#include "DbNVManager.h"
std::string CCurrentTime::GetCurrentTimeStringUTC()
{
	
	time_t now;
	struct tm* time_tm;
	time(&now);
	time_tm = gmtime(&now);
	
	return asctime(time_tm);
	
}

std::string CCurrentTime::GetCurrentTimeStringLocal()
{

	time_t now;
	struct tm* time_tm;
	time(&now);
	time_tm = localtime(&now);
	
	return asctime(time_tm);
}

CDatabaseConnection::CDatabaseConnection( CNVDataAccess* adapter )
{
	TRACE_IN();
	
	adapter_ = adapter;
	is_long_connection_ = true;
	create_time_ = CCurrentTime::GetCurrentTimeStringLocal();
	last_active_time_ = create_time_;
	life_count_ = LIFE_COUNT_Default;
	reference_count_ = 0;
	
	TRACE_OUT();
}

CDatabaseConnection::CDatabaseConnection( CNVDataAccess* adapter, const UINT32 life_count )
{
	TRACE_IN();
	
    adapter_ = adapter;
	is_long_connection_ = false;
	create_time_ = CCurrentTime::GetCurrentTimeStringLocal();
	last_active_time_ = create_time_;
	life_count_ = life_count;
	reference_count_ = 0;
	
	TRACE_OUT();
}

CDatabaseConnection::~CDatabaseConnection()
{

}

UINT64 CDatabaseConnection::GetKey()
{
	return (UINT64) adapter_;
}

void CDatabaseConnection::Lock()
{
	TRACE_IN();
	
	Activate();

	std::lock_guard<std::mutex> lg(reference_count_lock_);	
	reference_count_++;
	
	TRACE_OUT();
}

void CDatabaseConnection::Unlock()
{
	TRACE_IN();
	
	std::lock_guard<std::mutex> lg(reference_count_lock_);
	reference_count_--;
	
	TRACE_OUT();
}

void CDatabaseConnection::Activate()
{
	last_active_time_ = CCurrentTime::GetCurrentTimeStringLocal();
}


void CDatabaseConnection::DecreaseLife()
{	
	if(is_long_connection_)
	{
		return;
	}
	life_count_ --;
}

bool CDatabaseConnection::IsExpired()
{
	TRACE_IN();
	
	if(is_long_connection_)
	{
		return false;
	}
	
	if(IsLock())
	{
		return false;
	}
	if(life_count_ > 0)
	{
		return false;
	}
	return true;
}

bool CDatabaseConnection::IsLock()
{
	std::lock_guard<std::mutex> lg(reference_count_lock_);
	if(reference_count_>0)
	{
		Activate();
		return true;
	}
	
	return false;
}

CDbNVManager *CDbNVManager::m_sInstance = 0;

CDbNVManager::CDbNVManager()
{
	
}

CDbNVManager::~CDbNVManager()
{
	
}

CDbNVManager* CDbNVManager::Instance()
{	
	if(m_sInstance == 0)
	{
		m_sInstance = new CDbNVManager();
		
	}
	return m_sInstance;
}

int CDbNVManager::SetDNS(const char *pcHost,const char *pcDbname,const char *pcUsername,const char *pcPwd)
{
	
	if(0 == pcDbname || 0 == pcUsername || 0 == pcPwd)
	{
		return -1;
	}
	m_strHost = pcHost;
	m_strDbname = pcDbname;
	m_strUsername = pcUsername;
	m_strPwd = pcPwd;
	
	return 0;
}

CNVDataAccess* CDbNVManager::GetNVDataAccess()
{
	TRACE_IN();
	
	std::lock_guard<std::mutex> lg(m_pConnectLock);
	if(m_mapConnection.size() <= 0)
	{
		CNVDataAccess* pNVDataAccess = NULL;
		try
		{
			pNVDataAccess = CreateNVDataAccess();
			
		}
		catch(...)
		{
			return NULL;
		}
		
		if(NULL == pNVDataAccess)
		{
			return NULL;
		}
		
		CDatabaseConnection *database_connection = new CDatabaseConnection(pNVDataAccess);
		UINT64 key = database_connection->GetKey();
		m_mapConnection[key] = database_connection;		
		
		database_connection->Lock();
		return pNVDataAccess;
	}
	
	for(std::map<UINT64,CDatabaseConnection *>::iterator pos = m_mapConnection.begin();\
		pos != m_mapConnection.end();pos++)
		{
			CDatabaseConnection *connectionInfo = pos->second;
			assert(connectionInfo != NULL);
			if(!connectionInfo->IsLock())
			{
				connectionInfo->Lock();
				return connectionInfo->GetAdapter();
			}
		}
	CNVDataAccess* pNVDataAccess = NULL;
	try
	{
		pNVDataAccess = CreateNVDataAccess();
	}
	catch(...)
	{
		return NULL;
	}
	
	if(NULL == pNVDataAccess)
	{
		return NULL;
	}
	
	CDatabaseConnection *database_connection = new CDatabaseConnection(pNVDataAccess,CDatabaseConnection::LIFE_COUNT_Default);
	UINT64 key = database_connection->GetKey();
	m_mapConnection[key] = database_connection;	
	database_connection->Lock();
	
	TRACE_OUT();
	return pNVDataAccess;
}

void CDbNVManager::ReleaseNVDataAccess(CNVDataAccess* pNVDataAccess)
{
	TRACE_IN();
	
	UINT64 nKey = (UINT64)pNVDataAccess;
	std::lock_guard<std::mutex> lg(m_pConnectLock);
	std::map<UINT64,CDatabaseConnection*>::iterator pos = m_mapConnection.find(nKey);
	if(pos != m_mapConnection.end())
	{
		CDatabaseConnection *database_connection = pos->second;
		assert(database_connection != NULL);
		database_connection->Unlock();
		pNVDataAccess->FreeResult();
	}
	TRACE_OUT();
}


CNVDataAccess* CDbNVManager::CreateNVDataAccess()
{
	TRACE_IN();
	
	CNVDataAccess* pNVDataAccess = new CNVDataAccess;
	if(NULL == pNVDataAccess)
	{
		LOG_ERROR("Create AccessObject failed");
		return NULL;
	}
	try
	{
		pNVDataAccess->Connect(m_strHost.c_str(),m_strDbname.c_str(),m_strUsername.c_str(),m_strPwd.c_str());
	}
	catch(...)
	{
		LOG_ERROR("connect database failed");
		SafeDelete(pNVDataAccess);
		return NULL;
	}	
	TRACE_OUT();
	return pNVDataAccess;	
}


void CDbNVManager::Release()
{
	TRACE_IN();
	
	std::lock_guard<std::mutex> lg(m_pConnectLock);
	for(std::map<UINT64,CDatabaseConnection *>::iterator pos = m_mapConnection.begin();pos!= m_mapConnection.end();)
	{
		CDatabaseConnection* database_connection = pos->second;
		assert(database_connection != NULL);
		
		if(!database_connection->IsLock())
		{
			std::map<UINT64,CDatabaseConnection*>::iterator posCurrent = pos;
			pos++;
			m_mapConnection.erase(posCurrent);
			
			CNVDataAccess *adapter = database_connection->GetAdapter();
			assert(adapter != NULL);
			SafeDelete(database_connection);
			
			try
			{
				adapter->Disconnect();
			
			}
			catch(...)
			{
				
			}
		}
		else
		{
			pos++;
		}
		m_mapConnection.clear();
	}
	TRACE_OUT();
}

extern "C" int DbaModule_Initialize(
								const char *pcHost,
								const char *pcDbname,
								const char *pcUsername,
								const char *pcPwd
								)
{

    int res = CDbNVManager::Instance()->SetDNS(pcHost,pcDbname,pcUsername,pcPwd);
	if(0 != res)
		return res;

	CNVDataAccess *access = DbaModule_GetNVDataAccess();
	if(NULL == access) 
		return -1
    
	char *sql1 = "update wis_user_tbl set `status`=0";
	if(0 != access->ExecuteNonQuery(sql1))
	{
		LOG_ERROR("set user login status failed");
		DbaModule_ReleaseNVDataAccess(access);
		return -1;
	}
	char *sql2 = "update wis_device_tbl set `status`=0";
	if(0 != access->ExecuteNonQuery(sql2))
	{
		LOG_ERROR("set device login status failed");
		DbaModule_ReleaseNVDataAccess(access);
		return -1;
	}	
	DbaModule_ReleaseNVDataAccess(access);
	return 0;
	
	
}

extern "C" void* DbaModule_GetNVDataAccess()
{
	try
	{
		return CDbNVManager::Instance()->GetNVDataAccess();
	}
	catch ( ... )
	{
		LOG_ERROR("get database accesser failed");
	}
	return NULL;
}

extern "C" void DbaModule_ReleaseNVDataAccess(void * pNVDataAccess)
{
	try
	{
		CDbNVManager::Instance()->ReleaseNVDataAccess((CNVDataAccess*)pNVDataAccess);
	}
	catch ( ... )
	{
		//TraceError( "Catch exception at " << __FILE__ << ":" << __LINE__ );
	}
}

extern "C" int DbaModule_UnInitialize()
{
	try
	{
		CDbNVManager::Instance()->Release();
	}
	catch ( ... )
	{
		
		return -1;
	}

	return 0;
}



