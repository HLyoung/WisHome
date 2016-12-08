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
    CDevice *pDevice = GetDeviceClient(bus_address);
	if (pDevice != NULL){	
		std::string uuid = pDevice->GetUuid();
		int count = CountByUuid(uuid);
	    if(pDevice->IsLogined() && count < 2)
			  CUniteDataModule::GetInstance()->ShowClientDisConnect(bus_address,uuid, pDevice->GetLoginType());				
		pDevice->SetLogined(false);
		pDevice->SetDeviceExpire(true);
		return GetTCPServiceModuleInstance()->StopService(bus_address);
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

 	CDevice * pGatewayDevice = GetDeviceClient(&addr);
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
    string addresskey = GetAddressKey(bus_address);
    CDevice* pGatewayDevice = GetDeviceClient(bus_address);
    if (!pGatewayDevice){
		pGatewayDevice = new CDevice(this,bus_address);
		m_mapDevice.insert(pair<string,CDevice*>(addresskey,pGatewayDevice));
    	}
	CUniteDataModule::GetInstance()->ShowClientConnect(bus_address);

	TRACE_OUT();
}

string CDeviceManager::GetAddressKey(BUS_ADDRESS_POINTER  address)
{
	char tmp[128];
	memset(tmp,0,128);

	switch( address->bus_address_type )
	{
	case BUS_ADDRESS_TYPE_TCP:
	case BUS_ADDRESS_TYPE_UDP:
		{	
			sprintf(tmp,"%s-%u",address->host_address.ip, address->host_address.port);
		}
		break;
	default:
		break;
	}
    string key = string(tmp);
	return key;
}

CDevice* CDeviceManager::GetDeviceClient(BUS_ADDRESS_POINTER  address)
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

int  CDeviceManager::CountByUuid(const std::string & uuid)
{
	int count = 0;
	std::lock_guard<std::mutex> lg(m_Device_mutex);
	map<string,CDevice*>::iterator tor = m_mapDevice.begin();
	for(;tor != m_mapDevice.end();tor++)
			if(tor->second->GetUuid() == uuid)
				count++;
	return count;
}


void CDeviceManager::OnDisconnect(UINT32 size, void* data )
{
	TRACE_IN();
	
	BUS_ADDRESS_POINTER bus_address = (BUS_ADDRESS_POINTER) data;
	CDevice *pDevice = GetDeviceClient(bus_address);
	if (NULL != pDevice){
		pDevice->SetDeviceExpire(true);
		pDevice->SetLogined(false);
		int count = CountByUuid(pDevice->GetUuid());
		if(pDevice->IsLogined()  && count < 2)
			CUniteDataModule::GetInstance()->ShowClientDisConnect(bus_address,pDevice->GetUuid(),pDevice->GetLoginType());
	}	
	TRACE_OUT();
}

void CDeviceManager::OnReceive( UINT32 size, void* data )
{
	RECEIVE_DATA_POINTER receive_data = (RECEIVE_DATA_POINTER)data;
	CDevice* pGatewayDevice = GetDeviceClient(receive_data->from);
	if (NULL == pGatewayDevice)
	{
		return;
	}

	pGatewayDevice->Receive(receive_data->data.size, (void*)(receive_data->data.data));

}

void CDeviceManager::OnSend( UINT32 size, void* data )
{
	
}

bool CDeviceManager::SendData(BUS_ADDRESS_POINTER busAddress, int nRole, int nDataType, char* pData, int nDataSize)
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



