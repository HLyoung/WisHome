

#include "device.h"

CDevice::CDevice()
{
	m_isLogined = false;	
}
CDevice::CDevice(CDeviceManager *pOwner,BUS_ADDRESS_POINTER address):m_pManageOwner(pOwner),m_isLogined(false),m_loginType(0)
{
	m_bExpire = false;	
	memset((char *)&m_DeviceAddress,0,sizeof(BUS_ADDRESS));
	memcpy((char *)&m_DeviceAddress,(char *)address,sizeof(BUS_ADDRESS));
	
	TParserCallBack tParserCallBack = {0};
	tParserCallBack.pOwner = this;
	tParserCallBack.pParserCallback = ParserCallback;
	m_pProtocolPaser = GetParseProtocolModuleInstance()->RegisteCallBack(tParserCallBack);
	
//	StartHeartBeat();
}


CDevice::~CDevice()
{
	StopHeartBeat();
	GetParseProtocolModuleInstance()->FreeCallBack(m_pProtocolPaser);
}

bool CDevice::Receive(UINT32 length,void *data)
{
	if(0 >= length)
	{
		return false;
	}

	return GetParseProtocolModuleInstance()->ParseSocketProtocol(m_pProtocolPaser,(char *)data,length);
}

bool CDevice::Send(INT32 nCmd, void* data, INT32 nDataSize)
{
	TRACE_IN();
//如果是注册命令，则将设备注册状态置为已经注册。
	if(WIS_CMD_LOGIN == nCmd  || WIS_CMD_USER_AUTO_LOGIN == nCmd)
	{	
		logResult *result = (logResult *)data;
		if(WIS_CMD_LOGIN == nCmd && result->result == 1)
		{
			LOG_INFO("device(uuid = %s) logined.",result->uuid);
			SetLogined(true);
			SetLoginType(TYPE_DEVICE);
			m_uuid = std::string(result->uuid);
			
			nDataSize = sizeof(int);
			
		}
		else if(WIS_CMD_USER_AUTO_LOGIN == nCmd && result->result == 0)
		{
			LOG_INFO("user(uuid = %s) logined.",result->uuid);
			SetLogined(true);
			SetLoginType(TYPE_USER);
			m_uuid = std::string(result->uuid);
			
			nDataSize = sizeof(int);
		}
	}
	
	UINT32 nOutBufferLen = 0;
	char *pOutBuffer = new char[nDataSize + 12];
	memset(pOutBuffer,0,nDataSize + 12);

	GetParseProtocolModuleInstance()->MakeProtocolForSocket(m_pProtocolPaser,\
	nCmd,(char *)data,nDataSize,pOutBuffer,&nOutBufferLen);
	
	bool bRet = false;
	if(nOutBufferLen == 0 || NULL == pOutBuffer)
	{
		LOG_INFO("Parameter illegal!");
		return bRet;
	}

	bRet = GetTCPServiceModuleInstance()->SendData(m_pManageOwner,(UINT8*)pOutBuffer,nOutBufferLen,&m_DeviceAddress);
	SafeDeleteArray(pOutBuffer);
	
	TRACE_OUT();
	
	return bRet;
}

void CDevice::ParserCallback(UINT32 wEvent, UINT32 wResultCode, UINT32 wDataLen,const char* pData,void *pOwner)
{

	TRACE_IN();
	
	CDevice* pDevice = (CDevice*)pOwner;
	if (NULL == pDevice)
	{
		return;
	}
    
	if(WIS_CMD_LOGIN != wEvent && WIS_CMD_USER_AUTO_LOGIN != wEvent && WIS_CMD_HEART_BEAT != wEvent 
	   && WIS_CMD_USER_REGIST != wEvent && WIS_CMD_USER_RESET_PASSWORD != wEvent && WIS_CMD_USER_HEART_BEAT != wEvent)
	{
		if(!pDevice->IsLogined())
		{
			LOG_INFO("received something(event=%d) from a unloggined device and it`s not heart beat   --will drop it.  ",wEvent);
			return;
		}
	}

	//直接在设备层处理心跳。
	if(WIS_CMD_HEART_BEAT == wEvent || WIS_CMD_USER_HEART_BEAT == wEvent)
	{

		LOG_INFO("heat beat response recvived");		
		pDevice->m_isGetHeartBeatResponse = true;
		pDevice->m_loseHeartBeatTimes = 0;
		return;
	}
	
	DEVICE_INFO devInfo;  
	memcpy((char *)&devInfo.bus_address,(const char *)(&(pDevice->m_DeviceAddress)),sizeof(BUS_ADDRESS));

	devInfo.is_logined = pDevice->IsLogined(); 
	devInfo.login_type = pDevice->GetLoginType();
	devInfo.uuid = pDevice->GetUuid();
	
	CUniteDataModule::GetInstance()->ShowNetDataToInterface(devInfo, wEvent,wResultCode, wDataLen, pData);

	TRACE_OUT();
}

void CDevice::GetBusAddress(BUS_ADDRESS & tBusAddr)
{
	memcpy((char *)&tBusAddr,(char *)&m_DeviceAddress,sizeof(BUS_ADDRESS));
}

void CDevice::UpdateAddressInfo(BUS_ADDRESS &tBusAddr)
{
	memcpy((char *)&m_DeviceAddress,(char *)&tBusAddr,sizeof(BUS_ADDRESS));
}

void CDevice::SetLogined(bool isLogined)
{
	m_isLogined = isLogined;
}

bool CDevice::IsLogined(void)
{
	return m_isLogined;
}


int CDevice::HeartBeratTimerHandler(void *device)
{
	TRACE_IN();
	
	if(NULL == device)
		return -1;
	CDevice *pDevice  = (CDevice *)device;

	if(!pDevice->m_isGetHeartBeatResponse)
		pDevice->m_loseHeartBeatTimes++;
	if(pDevice->m_loseHeartBeatTimes >= 3)
	{
		pDevice->m_pManageOwner->StopTcpServer(&(pDevice->m_DeviceAddress));
		pDevice->StopHeartBeat();
		LOG_INFO("lose heart beat response 10 times,delete the link(ip = %s,port = %d)",pDevice->m_DeviceAddress.host_address.ip,\
					pDevice->m_DeviceAddress.host_address.port);
		return -1;
	}
    int heartBeatCmd = 0;
	if(pDevice->m_loginType == TYPE_DEVICE)
		heartBeatCmd = WIS_CMD_HEART_BEAT;
	else
		heartBeatCmd = WIS_CMD_USER_HEART_BEAT;
	
	unsigned int current = (unsigned int )time(0);
	pDevice->Send(heartBeatCmd,(char *)&current,sizeof(int ));
	
    pDevice->m_isGetHeartBeatResponse = false;
	
	TRACE_OUT();
	return 0;
}

void CDevice::StartHeartBeat()
{
	TRACE_IN();
	if(this->m_DeviceAddress.model_type != TCP_SERVER_MODE)
	{
		LOG_INFO("only heart beat in server mode");
		return;
	}
	m_isGetHeartBeatResponse = false;
	m_loseHeartBeatTimes = 0;
	m_heartBeartTiemr.open(30,HeartBeratTimerHandler,this);

	TIMERMANAGER->Register(&m_heartBeartTiemr);
	
	TRACE_OUT();
}

void CDevice::StopHeartBeat()
{
	TIMERMANAGER->unRegister(&m_heartBeartTiemr);
}