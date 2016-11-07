

#include "TCPServiceModule.h"
CTCPServiceModule *CTCPServiceModule::m_pInstance = NULL;	

ICTCPServiceModule * GetTCPServiceModuleInstance()
{
	return CTCPServiceModule::GetInstance();
}

CTCPServiceModule::CTCPServiceModule()
{
	
}

CTCPServiceModule::~CTCPServiceModule()
{
	
}

CTCPServiceModule *CTCPServiceModule::GetInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CTCPServiceModule;
		
	}
	return m_pInstance;
}

bool CTCPServiceModule::StartTCPServer(void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback)
{
	return CTCPServiceManage::GetInstance()->StartServer(owner,nPort,callback);
}

bool CTCPServiceModule::ConnectGatewayServer(void* owner, string& strIp, int nPort, BUS_SERVICE_MESSAGE_CALLBACK callback)
{
	return CTCPServiceManage::GetInstance()->CreateTcpClient(owner, strIp, nPort, callback);
}


bool CTCPServiceModule::StopService(BUS_ADDRESS_POINTER pBusAddress)
{
	 CTCPServiceManage::GetInstance()->StopService(pBusAddress);
	 return true;
}

bool CTCPServiceModule::SendData(void* hHandle, UINT8* pData, UINT32 length, BUS_ADDRESS_POINTER send_argument)
{
	return CTCPServiceManage::GetInstance()->SendData(hHandle, (UINT8*)pData, length, send_argument);
}
