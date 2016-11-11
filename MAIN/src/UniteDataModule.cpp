#include "UniteDataModule.h"

/*
*	面向应用的导出接口
*/

extern "C" ICUniteDataModule* GetUniteDataModuleInstance()
{
	return CUniteDataModule::GetInstance();
}

CUniteDataModule* CUniteDataModule::m_Instance = NULL;
CUniteDataModule* CUniteDataModule::GetInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CUniteDataModule();
	}

	return m_Instance;
}

CUniteDataModule::CUniteDataModule(void) : m_pInterfaceCallback(NULL),m_bStarted(false)
{
}

CUniteDataModule::~CUniteDataModule(void)
{
}


bool CUniteDataModule::StartModule()
{
	if (m_bStarted)
	{
		std::cout<<"it is already started."<<std::endl;
		return true;
	}

	bool Result = true;
	bool bRet = true;
	//加载配置文件

	if (!LoadConfigFile())
	{
		std::cout<<"load config file filed."<<std::endl;
		Result = false;
	}

	
	if(!StartTimerModule())
	{
		std::cout<<"init timer module failed"<<std::endl;
		Result = false;
	}
	
	//LOG模块初始化
	
	if(!LogModuleInit())
	{
		std::cout<<"init log module failed."<<std::endl;
		Result = false;
	}
	
	//数据库模块初始化
	
	if(!DatabaseModuleInit())
	{
		std::cout<<"init database module failed."<<std::endl;
		Result = false;
	}	
	m_bStarted = true;
      
	return Result;
}

bool CUniteDataModule::StopModule()
{
	CZxLogManager *manager = CZxLogManager::getInstance();
	manager->DeInit();
	return true;
}

bool CUniteDataModule::StartTcpServer(int nPort)
{
	//启动TCP服务器
	return CDeviceManager::GetInstance()->StartTcpServer(nPort);
}

bool CUniteDataModule::StartTimerModule()
{
	if(TIMERMANAGER->get_instance()->start())
		return false;
	return true;
}


bool CUniteDataModule::RegistInterfacRecObject(IAutoReportToInterface* pInterfaceObj)
{
	if (NULL == pInterfaceObj)
	{
		return false;
	}

	m_pInterfaceCallback = pInterfaceObj;

	return true;
}

//连接服务器
bool CUniteDataModule::ConnectServer(string strServerIp,int nServerPort)
{
	return CDeviceManager::GetInstance()->CreateConnectToDevice(strServerIp,nServerPort);
}

//向tcp对端发送数据（服务器端或客户端）
bool CUniteDataModule::SendData(string uuid,int nDataType, char* pSendData, int nDataSize, int nRole)
{
	return CDeviceManager::GetInstance()->SendData(uuid,nRole,nDataType,pSendData,nDataSize);
}

bool CUniteDataModule::SendData(BUS_ADDRESS &busAddress,int nDataType, char* pSendData, int nDataSize, int nRole)
{
	return CDeviceManager::GetInstance()->SendData(busAddress,nRole,nDataType,pSendData,nDataSize);
}


//向应用回送数据
bool CUniteDataModule::ShowNetDataToInterface(DEVICE_INFO& GatewayInfo, int nDataType, int nResultCode, int nDataLen, const char* pData)
{
	if (!m_pInterfaceCallback)
	{
		return false;
	}

	m_pInterfaceCallback->ShowClientReceiveData(GatewayInfo, nDataType,nResultCode, nDataLen, pData);
	return true;
}

//向应用回调TCP连接状态
void CUniteDataModule::ShowClientConnect(BUS_ADDRESS GatewayAddress)
{
	if (!m_pInterfaceCallback)
	{
		return;
	}

	m_pInterfaceCallback->ShowClientConnect(GatewayAddress);
}

void CUniteDataModule::ShowClientDisConnect(BUS_ADDRESS GatewayAddress,std::string uuid,int loginType)
{
	if (!m_pInterfaceCallback)
	{
		return;
	}

	m_pInterfaceCallback->ShowClientDisConnect(GatewayAddress,uuid,loginType);
}
bool CUniteDataModule::LogModuleInit()
{
	CXmlDocument DBconfigXml;
	GetConfigXml(DBconfigXml);
	
	CXmlElement *pDocElement = DBconfigXml.RootElement();
	CXmlElement *pDBAccessElement = pDocElement->FirstChildElement("LogConfig");
	string configfile = pDBAccessElement->Attribute("configfile");
	string logger = pDBAccessElement->Attribute("logger");
	
	CZxLogManager *manager = CZxLogManager::getInstance();
	manager->Init(configfile.c_str(),logger.c_str());
	
	return true;
}

bool CUniteDataModule::DatabaseModuleInit()
{
	CXmlDocument DBconfigXml;
	GetConfigXml(DBconfigXml);
	
	CXmlElement *pDocElement = DBconfigXml.RootElement();
	CXmlElement *pDBAccessElement = pDocElement->FirstChildElement("DBAccess");
	
	string dbHostName = pDBAccessElement->Attribute("HostName");
	string dbName = pDBAccessElement->Attribute("dbName");
	string username = pDBAccessElement->Attribute("username");
	string pwd = pDBAccessElement->Attribute("pwd");
	
	if(-1 == DbaModule_Initialize(dbHostName.c_str(),dbName.c_str(),username.c_str(),pwd.c_str()))
	{
		LOG_ERROR("Init database failed.");
		return false;
	}
	return true;
}

bool CUniteDataModule::LoadConfigFile()
{
	TRACE_IN();
	int nLen = 256;
	char cPath[256] = {0};
	if(!getcwd(cPath,nLen))
	{
		std::cout<<"load config file failed."<<std::endl;
		return false;
	}
	string strPath(cPath);
	strPath += "/config/config.xml";
	
	if(0 != access(strPath.c_str(),F_OK))
	{
		std::cout<<"confige file doen`t exist!"<<std::endl;
		return false;
	}
	TRACE_OUT();
	
	return m_ConfigXmlFile.LoadFile(strPath.c_str());
}

void  CUniteDataModule::GetConfigXml(CXmlDocument &xmldocument)
{
	CSmartBuffer buffer;
	m_ConfigXmlFile.SaveFile(buffer);
	xmldocument.LoadFile(buffer);
}

IAutoReportToInterface* CUniteDataModule::GetInterfaceObj()
{
	return m_pInterfaceCallback;
}
