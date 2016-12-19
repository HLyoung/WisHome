
#include "TCPServiceManager.h"


CTCPServiceManage* CTCPServiceManage::m_instance = NULL;

CTCPServiceManage::CTCPServiceManage(void)
{
}

CTCPServiceManage::~CTCPServiceManage(void)
{
}

CTCPServiceManage *CTCPServiceManage::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new CTCPServiceManage;
	}
	return m_instance;
}

CTCPService *CTCPServiceManage::StartServer(void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback)
{
	TRACE_IN();
	
	CTCPService *pTcpserver = new CTCPService();
	pTcpserver->InitServer(owner,nPort,callback);
	
	CADDRINFO addr("0.0.0.0",pTcpserver->GetPort());
	if(0 > GetCommonTCPManager()->BeginServer(addr,pTcpserver))
	{
		LOG_ERROR("common tcp manager begin tcp server failed Port = %d",nPort);
		SafeDelete(pTcpserver);
		return NULL;
	}
	
	TRACE_OUT();
	return pTcpserver;
}

CTCPService* CTCPServiceManage::CreateTcpClient(void* owner,string &strIp,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback)
{
	TRACE_IN();
	CTCPService *pTcpserver = new CTCPService();
	pTcpserver->InitClient(owner,strIp,nPort,TCP_CLIENT_MODE,callback);
	
	CADDRINFO addr(strIp.c_str(),nPort);
	if(0 > GetCommonTCPManager()->ConnectServer(addr,pTcpserver,TCP_CONNECT_LONG,60*60))
	{
		LOG_ERROR("connnect server failed .ip = %s,port = %d",strIp.c_str(),nPort);
		return NULL;
	}
	TRACE_OUT();
	return pTcpserver;
}

void CTCPServiceManage::StopService(BUS_ADDRESS_POINTER pBusAddress)
{

	TRACE_IN();
	RemoveTcpService(pBusAddress);
	TRACE_OUT();
}

void CTCPServiceManage::NotifyMessageConnect(CTCPService *pTcpService,void* handle,BUS_ADDRESS_POINTER pBusAddress)
{
	if(NULL == pTcpService)
	{
		LOG_INFO("illegal parameter");
		return;
	}
	SaveLinkAndCallBack(pBusAddress,pTcpService,handle);
}

void CTCPServiceManage::GetLinkAddressInfo(CTCPService* pTcpService,HOST_ADDRESS &address,BUS_ADDRESS_POINTER pBusAddress)
{
	memcpy((char*)address.ip,pBusAddress->host_address.ip,HOST_NAME_LENGTH);
	address.port = pBusAddress->host_address.port;
	address.size = sizeof(HOST_ADDRESS);
}

void CTCPServiceManage::SaveLinkAndCallBack(BUS_ADDRESS_POINTER pBus_address,CTCPService *pTcpService,void* handle)
{
	TRACE_IN();
	bool done = CHostAddressMap::SetHostAddress(&(pBus_address->host_address),handle);
	if(!done)
	{
		LOG_ERROR("set host address failed");
		return;
	}
	
	if(pTcpService->message_callback_ != NULL)
	{
		LOG_INFO("tcp service manager call the callback funciton to notify connection fd = %ld",(long)handle);
		pTcpService->message_callback_(pTcpService->GetOwnerHandle(),MESSAGE_Connect,sizeof(BUS_ADDRESS),pBus_address);
	}

	std::string key = tcpGetAddressKey(pBus_address);
	std::lock_guard<std::mutex> lg(m_tcpLinkMutex);
	m_mapTcpLink.insert(pair<std::string,CTCPService *>(key,pTcpService));

	TRACE_OUT();
}

void CTCPServiceManage::NotifyMessageDisconnect(CTCPService *pTcpService,void* handle,BUS_ADDRESS_POINTER pBus_address)
{
	if(NULL == pTcpService)
	{
		return;
	}
	DeleteLinkAndCallBack(pBus_address,pTcpService,handle);
}

//删除链路信息和向调用层回调通知消息
void CTCPServiceManage::DeleteLinkAndCallBack(BUS_ADDRESS_POINTER pBus_address,CTCPService* pTcpService, void*handle)
{
	TRACE_IN();
	if (pTcpService->message_callback_ != NULL)
	{
		pTcpService->message_callback_ ( pTcpService->GetOwnerHandle(), MESSAGE_Disconnect, sizeof(BUS_ADDRESS), pBus_address );
	}
	string key_string = CHostAddress::GetKey((const char*)pBus_address->host_address.ip,pBus_address->host_address.port);
	CHostAddressMap::RemoveHostAddress(key_string,false);

	std::string key = tcpGetAddressKey(pBus_address);
	std::lock_guard<std::mutex> lg(m_tcpLinkMutex);
	std::map<std::string,CTCPService *>::iterator iter = m_mapTcpLink.find(key);
	if(iter != m_mapTcpLink.end())
	{
		SafeDelete(iter->second);
		m_mapTcpLink.erase(key);
	}
	TRACE_OUT();
}

//（服务器端/客户端）接收到对端数据
void CTCPServiceManage::NotifyMessageReceiveData(CTCPService* pTcpService,void*handle,const char *pData,int nLen,BUS_ADDRESS_POINTER pBus_address)
{
	TRACE_IN();
	RECEIVE_DATA_POINTER recv_data = new RECEIVE_DATA;
	recv_data->from = pBus_address;
	recv_data->data.size = nLen;
	recv_data->data.data = pData;

	if (pTcpService->message_callback_ != NULL)
	{
		pTcpService->message_callback_ (pTcpService->GetOwnerHandle(), MESSAGE_Receive,nLen, recv_data);
	}
	SafeDelete(recv_data);
	TRACE_OUT();
}

//（服务器端/客户端）主动发送数据到对端
bool CTCPServiceManage::SendData(void* hHandle, UINT8* data, UINT32 length, BUS_ADDRESS_POINTER bus_address)
{
	CTCPService *pTCPService = (CTCPService *)hHandle;

	string key_string = CHostAddress::GetKey((const char*) bus_address->host_address.ip, bus_address->host_address.port );
	const void* handle = CHostAddressMap::GetHostAddress( key_string );
	if (NULL != handle ){
		if(GetCommonTCPManager()->Send((void*)handle, (const char *)data, length))
			return true;
		}
	return false;
}

//只支持作为客户端主动删除tcp通信对象
void CTCPServiceManage::RemoveTcpService(BUS_ADDRESS_POINTER pBus_address)
{
	TRACE_IN();
	CHostAddressMap::RemoveHostAddress(pBus_address->host_address);

	std::string key = tcpGetAddressKey(pBus_address);
	std::lock_guard<std::mutex> lg(m_tcpLinkMutex);
	std::map<std::string,CTCPService *>::iterator ite = m_mapTcpLink.find(key);
	if(ite == m_mapTcpLink.end()){
		LOG_ERROR("the link(ip = %s,port = %d about to delete not found",pBus_address->host_address.ip,pBus_address->host_address.port);
		return;
	}
	SafeDelete(ite->second);
	m_mapTcpLink.erase(ite);
		
	TRACE_OUT();
}  


string CTCPServiceManage::tcpGetAddressKey(BUS_ADDRESS_POINTER pAddress)
{
	char tmp[128];
	memset(tmp,0,128);

	switch( pAddress->bus_address_type )
	{
	case BUS_ADDRESS_TYPE_TCP:
	case BUS_ADDRESS_TYPE_UDP:
		{	
			sprintf(tmp,"%s-%u",pAddress->host_address.ip, pAddress->host_address.port);
		}
		break;
	default:
		break;
	}
    string key = string(tmp);
	return key;
}


