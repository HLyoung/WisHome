#include "deviceManager.h"


CDeviceManager * CDeviceManager::m_pInstance = NULL;
CDeviceManager::CDeviceManager(void)
{
	memset(&m_tInitialArguments,0,sizeof(INITIAL_ARGUMENTS));
	m_tInitialArguments.owner = (HANDLE)this;
	m_tInitialArguments.net_message_callback = MessageCallback;
	m_bInitScan = false;
}

CDeviceManager::~CDeviceManager(void)
{
}

CDeviceManager* CDeviceManager::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CDeviceManager();
	}
	return m_pInstance;
}

bool CDeviceManager::StartTcpServer(int nPort)
{
	TRACE_IN();
	if (0 >= nPort)
	{
		LOG_INFO("when start tcp server ,nport <= 0,geting  it from xlm file.");
		nPort = GetTcpServicePort();
	}

	if (0 >= nPort)
	{
		LOG_ERROR("when start tcp server ,read port from xml failed~! nPort = %d",nPort);
		return false;
	}

	TRACE_OUT();
	return GetTCPServiceModuleInstance()->StartTCPServer(this,nPort,MessageCallback);
}

bool CDeviceManager::ScanAllDevice()
{
	
}


int CDeviceManager::GetTcpServicePort()
{
}

bool CDeviceManager::StopTcpServer(BUS_ADDRESS_POINTER pBusAddress)
{	
	OnDisconnect(sizeof(BUS_ADDRESS),pBusAddress);
	return GetTCPServiceModuleInstance()->StopService(pBusAddress);
}

bool CDeviceManager::CreateConnectToDevice(string& strIp,int nPort)
{
	TRACE_IN();
	LOG_INFO("create connect to device ip = %s,port = %d",strIp.c_str(),nPort);
	
	BUS_ADDRESS addr;
	memcpy((char*)addr.host_address.ip, strIp.c_str(), strIp.length());
    addr.bus_address_type = BUS_ADDRESS_TYPE_TCP;
	addr.model_type = TCP_CLIENT_MODE;
	addr.host_address.port = nPort;

 	CDevice * pGatewayDevice = GetDeviceClient(addr);
	if (NULL == pGatewayDevice)
	{
		return GetTCPServiceModuleInstance()->ConnectGatewayServer(this, strIp, nPort, MessageCallback);
	}
	
	TRACE_OUT();
	return true;
}

void CDeviceManager::MessageCallback( HANDLE owner, UINT32 message, UINT32 message_size, void* message_data )
{
	CDeviceManager* pGatewayDeviceManage = (CDeviceManager*)owner;
	if (NULL == pGatewayDeviceManage)
	{
		return;
	}

	switch ( message )
	{
	case MESSAGE_Connect:
		pGatewayDeviceManage->OnConnect( message_size, message_data );
		break;

	case MESSAGE_Disconnect:
		pGatewayDeviceManage->OnDisconnect( message_size, message_data );
		break;

	case MESSAGE_Receive:
		pGatewayDeviceManage->OnReceive( message_size, message_data );
		break;

	case MESSAGE_Sent:
		pGatewayDeviceManage->OnSend( message_size, message_data );
		break;

	default:
		break;
	}
}



void CDeviceManager::OnConnect( UINT32 size, void* data )
{

	TRACE_IN();
	BUS_ADDRESS_POINTER bus_address = (BUS_ADDRESS_POINTER) data;
	if (NULL == bus_address)
	{
		LOG_INFO("OnConnect callback argument error!");
		return;
	}

    string addresskey = GetAddressKey(*bus_address);

    if (addresskey.length() == 0)
    {
        LOG_INFO("RemoveGateway address key is empty");
        return;
    }

    CDevice* pGatewayDevice = GetDeviceClient(*bus_address);
    if (!pGatewayDevice)
	{
		pGatewayDevice = new CDevice(this,bus_address);
		
		m_Device_mutex.lock();
		m_mapDevice.insert(pair<string,CDevice*>(addresskey,pGatewayDevice));
		m_Device_mutex.unlock();
	}

	CUniteDataModule::GetInstance()->ShowClientConnect(*bus_address);

	TRACE_OUT();
}

string CDeviceManager::GetAddressKey(BUS_ADDRESS& address)
{
	char tmp[128];
	memset(tmp,0,128);

	switch( address.bus_address_type )
	{
	case BUS_ADDRESS_TYPE_TCP:
	case BUS_ADDRESS_TYPE_UDP:
		{	
			sprintf(tmp,"%s-%u",address.host_address.ip, address.host_address.port);
		}
		break;
	default:
		break;
	}
    string key = string(tmp);
	return key;
}

CDevice* CDeviceManager::GetDeviceClient(BUS_ADDRESS& address)
{
	TRACE_IN();
	string address_key = GetAddressKey(address);

	std::lock_guard<std::mutex> lg(m_Device_mutex);	
	map<string,CDevice*>::iterator ite = m_mapDevice.find(address_key);
	if (ite == m_mapDevice.end())
	{
		return NULL;
		
	}	
	TRACE_OUT();
	return (CDevice*)ite->second;
}

CDevice* CDeviceManager::GetDeviceClient(std::string uuid)
{
	TRACE_IN();
	std::lock_guard<std::mutex> lg(m_Device_mutex);

	map<string,CDevice*>::iterator ite = m_mapDevice.begin();
	for( ;ite != m_mapDevice.end();ite++)
	{
		if(ite->second->GetUuid() == uuid  && ite->second->IsLogined()) 
		{
			return ite->second;
		}
	}	
	TRACE_OUT();
	return NULL;
}

void CDeviceManager::OnDisconnect(UINT32 size, void* data )
{
	TRACE_IN();
	BUS_ADDRESS_POINTER bus_address = (BUS_ADDRESS_POINTER) data;
	if (NULL == bus_address)
	{
		LOG_INFO("OnDisconnect callback argument error!");
		return;
	}

	string addresskey = GetAddressKey(*bus_address);	
    std::lock_guard<std::mutex> lg(m_Device_mutex);
	map<string,CDevice*>::iterator ite = m_mapDevice.find(addresskey);	
	if (ite == m_mapDevice.end()){
		LOG_INFO("the link doesn`t found");
		return;
		}
	
	CDevice* pGatewayDevice = (CDevice*)ite->second;
	if (NULL == pGatewayDevice){
		LOG_INFO("the device doesn`t found");
		return;
		}
	
	std::string uuid = pGatewayDevice->GetUuid();
	int loginType = pGatewayDevice->GetLoginType();
	
	SafeDelete(ite->second);
	m_mapDevice.erase(ite->first);	
	
	for(ite = m_mapDevice.begin();ite != m_mapDevice.end();ite ++){  //当一个设备有多个连接的时候，某一个连接断了并不将设备置为离线。
		if(ite->second->GetUuid() == uuid){
			return;
		}
	}
		
    CUniteDataModule::GetInstance()->ShowClientDisConnect(*bus_address,uuid,loginType);
	TRACE_OUT();
}

void CDeviceManager::OnReceive( UINT32 size, void* data )
{
	RECEIVE_DATA_POINTER receive_data = (RECEIVE_DATA_POINTER)data;
	CDevice* pGatewayDevice = GetDeviceClient(*(receive_data->from));


	if (NULL == pGatewayDevice)
	{
		return;
	}

	pGatewayDevice->Receive(receive_data->data.size, (void*)(receive_data->data.data));

}

void CDeviceManager::OnSend( UINT32 size, void* data )
{
	LOG_INFO("send data successful");
}

bool CDeviceManager::SendData(BUS_ADDRESS &busAddress, int nRole, int nDataType, char* pData, int nDataSize)
{
	TRACE_IN();
	
	CDevice* pGatewayDevice = GetDeviceClient(busAddress);
	if (NULL != pGatewayDevice)
	{
		return pGatewayDevice->Send(nDataType, pData, nDataSize);
	}

	TRACE_OUT();
	return false;
	
}

bool CDeviceManager::SendData(std::string uuid,int nRole,int nDataType,char *pData,int nDataSize)
{
	//当根据设备ID发送数据时，strIp是设备ID
	TRACE_IN();

	bool res = false;
	int count = 0;
   
	map<string,CDevice*>::iterator ite = m_mapDevice.begin();
	for( ;ite != m_mapDevice.end();ite++)
	{
		if(ite->second->GetUuid() == uuid  && ite->second->IsLogined()) 
		{
			res = ite->second->Send(nDataType,pData,nDataSize);
			count ++;			
		}

	}
	LOG_INFO("WOWWOWWOW  device(uuid = %s) has %d connects",uuid.c_str(),count);
	TRACE_OUT();
	return res;
}

