

#include "TCPService.h"
#include "TCPServiceManager.h"


CTCPService::CTCPService()
{
	m_timeout = default_time_out;
}

CTCPService::~CTCPService()
{
	
}

bool CTCPService::InitServer( void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback)
{
	owner_ = owner;
	message_callback_ = callback;
	m_port = nPort;
	m_timeout = default_time_out;
	m_TcpServiceMode = TCP_SERVER_MODE;
	m_bValid = true;
	m_strIp = "";
	return true;
}

bool CTCPService::InitClient(void* owner,string strIp,int nPort,int nTCPServiceType,BUS_SERVICE_MESSAGE_CALLBACK callback )
{
	owner_ = owner;
	message_callback_ = callback;
	m_bValid = true;
	m_timeout = default_time_out;
	m_TcpServiceMode = nTCPServiceType;
	m_strIp = strIp;
	m_port = nPort;
	return true;
}

CISocketOwner *CTCPService::OnAccept(void* handle,BUS_ADDRESS_POINTER pBus_address)
{
	CTCPService *pTcpService = new CTCPService;
	pTcpService->InitServer(owner_,m_port,message_callback_);
	pTcpService->SetServiceModel(pBus_address->model_type);
	pTcpService->SetIp((char *)pBus_address->host_address.ip);
	
	CTCPServiceManage::GetInstance()->NotifyMessageConnect(pTcpService,handle,pBus_address);
	return pTcpService;
}

int CTCPService::OnConnect(void* handle,int OwnerType,BUS_ADDRESS_POINTER pBus_address)
{
	CTCPServiceManage::GetInstance()->NotifyMessageConnect(this,handle,pBus_address);
	return 0;
}

int CTCPService::OnClose(void*handle,BUS_ADDRESS_POINTER pBus_address)
{
	CTCPServiceManage::GetInstance()->NotifyMessageDisconnect(this,handle,pBus_address);
	return 0;
}

int CTCPService::OnCmdCome(void* handle,const char *pData,int nLen,BUS_ADDRESS_POINTER pBus_address)
{
	CTCPServiceManage::GetInstance()->NotifyMessageReceiveData(this,handle,pData,nLen,pBus_address);
	return 0;
}

int CTCPService::OnError(void *handle,int & nErrorNo,BUS_ADDRESS_POINTER pBus_address)
{
	return 0;
}

int CTCPService::OnTimeOut(void *handle,BUS_ADDRESS_POINTER pBus_address)
{
	return 0;
}

int CTCPService::Release()
{
	return 0;
}

int CTCPService::GetServiceModel()
{
	return m_TcpServiceMode;
}

void CTCPService::SetServiceModel(int nModel)
{
	m_TcpServiceMode = nModel;
}


int CTCPService::GetTimeout()
{
	return m_timeout;
}

void* CTCPService::GetOwnerHandle()
{
	return owner_;
}

void CTCPService::SetTcpServiceValid(bool bValid)
{
	std::lock_guard<std::mutex> locVal(m_ValidMutex);
	m_bValid = bValid;
}

bool CTCPService::GetTcpServiceValid()
{
	std::lock_guard<std::mutex> locVal(m_ValidMutex);
	return m_bValid;
}






