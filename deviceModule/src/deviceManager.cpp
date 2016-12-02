#include "deviceManager.h"


CDeviceManager * CDeviceManager::m_pInstance = NULL;
CDeviceManager::CDeviceManager(void)
{
	memset(&m_tInitialArguments,0,sizeof(INITIAL_ARGUMENTS));
	m_tInitialArguments.owner = (HANDLE)this;
	m_tInitialArguments.net_message_callback = MessageCallback;

	StartClearTimer();
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

bool CDeviceManager::StopTcpServer(BUS_ADDRESS_POINTER bus_address)
{	
	string addresskey = GetAddressKey(*bus_address);
		
	std::lock_guard<std::mutex> lg(m_Device_mutex);
	map<string,CDevice*>::iterator ite = m_mapDevice.find(addresskey);	
	if (ite != m_mapDevice.end()){	
		CDevice* pGatewayDevice = (CDevice*)ite->second;
		if (NULL != pGatewayDevice){		
			std::string uuid = pGatewayDevice->GetUuid();
			int count = 0;
			for(ite = m_mapDevice.begin();ite != m_mapDevice.end();ite ++){  //当一个用户在多地登录时，一个用户下线，不把用户设为未登录状态。
				if(ite->second->GetUuid() == uuid){
					count++;
				}
			}
			if(count <= 1)
				if(pGatewayDevice->IsLogined())
			   		CUniteDataModule::GetInstance()->ShowClientDisConnect(*bus_address,uuid, pGatewayDevice->GetLoginType());
				
			pGatewayDevice->SetLogined(false);
			pGatewayDevice->SetDeviceExpire(true);
			return GetTCPServiceModuleInstance()->StopService(bus_address);
		}
    }
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
		return NULL;
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
	ite->second->SetDeviceExpire(true);                        //set device expire and delete it in the timer.
	
	for(ite = m_mapDevice.begin();ite != m_mapDevice.end();ite ++){  //当一个设备有多个连接的时候，某一个连接断了并不将设备置为离线。
		if(ite->second->GetUuid() == uuid){
			LOG_INFO("use or device(uuid = %s) has more than one link",uuid.c_str());
			return;
		}
	}
		
    CUniteDataModule::GetInstance()->ShowClientDisConnect(*bus_address,uuid,pGatewayDevice->GetLoginType());
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
	if (NULL != pGatewayDevice  )
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
	std::lock_guard<std::mutex> lg(m_Device_mutex);
	map<string,CDevice*>::iterator ite = m_mapDevice.begin();
	for( ;ite != m_mapDevice.end();ite++)
		if(ite->second->GetUuid() == uuid  && ite->second->IsLogined()) 
			ite->second->Send(nDataType,pData,nDataSize);		
	TRACE_OUT();
	return true;
}

void CDeviceManager::StartClearTimer()
{
	m_tClearExpireDevice.open(1,ClearDeviceTimerHandler,(void*)this);
	TIMERMANAGER->Register(&m_tClearExpireDevice);	
}

int CDeviceManager::ClearDeviceTimerHandler(void * manager)
{
	CDeviceManager* pManager = (CDeviceManager *)manager;
	std::lock_guard<std::mutex> lg(pManager->m_Device_mutex);
	std::map<string,CDevice*>::iterator ite = pManager->m_mapDevice.begin();
	while(ite != pManager->m_mapDevice.end()){
		if(ite->second->GetDeviceExpire()){
			SafeDelete(ite->second);			
			pManager->m_mapDevice.erase(ite++);
			continue;
			}
		ite++;
		}
}

void CDeviceManager::HandlerUserMultipleLogin( const std::string& uuid)  //logout the user who has the same id,wait for user to disconnect.
{	
	TRACE_IN();
	std::lock_guard<std::mutex> lg(m_Device_mutex);
	std::map<string,CDevice*>::iterator ite = m_mapDevice.begin();
	for(;ite != m_mapDevice.end();ite ++)
		if(uuid == ite->second->GetUuid() && ite->second->IsLogined()){
			ite->second->Send(WIS_CMD_SERVICE_KICKOUT_USER,NULL,0);
			ite->second->Send(WIS_CMD_SERVICE_KICKOUT_USER,NULL,0);
			ite->second->SetLogined(false);
			}
	TRACE_OUT();
}

