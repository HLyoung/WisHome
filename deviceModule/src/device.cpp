

#include "device.h"

CDevice::CDevice()
{
	m_isLogined = false;	
}
CDevice::CDevice(CDeviceManager *pOwner,BUS_ADDRESS_POINTER address):m_pManageOwner(pOwner),m_isLogined(false),m_loginType(0)
{
	m_bExpire = false;	
	m_DeviceAddress = address;
	
	TParserCallBack tParserCallBack = {0};
	tParserCallBack.pOwner = this;
	tParserCallBack.pParserCallback = ParserCallback;
	m_pProtocolPaser = GetParseProtocolModuleInstance()->RegisteCallBack(tParserCallBack);
	
	StartHeartBeat();
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
		    LOG_INFO("device(uuid = %s logined",result->uuid);
			SetLogined(true);
			SetLoginType(TYPE_DEVICE);
			m_uuid = getUuidFromBuffer(result->uuid);
			nDataSize = sizeof(int);
			
		}
		else if(WIS_CMD_USER_AUTO_LOGIN == nCmd && result->result == 0)
		{
			LOG_INFO("user(uuid = %s) logined.",result->uuid);
			SetLogined(true);
			SetLoginType(TYPE_USER);	
			m_uuid = getUuidFromBuffer(result->uuid);
			nDataSize = sizeof(int);
		}
	}
	if(WIS_CMD_SERVICE_KICKOUT_USER)
		this->SetLogined(false);
	
	UINT32 nOutBufferLen = 0;
	char *pOutBuffer = new char[nDataSize + 12];
	memset(pOutBuffer,0,nDataSize + 12);

	GetParseProtocolModuleInstance()->MakeProtocolForSocket(m_pProtocolPaser,\
	nCmd,(char *)data,nDataSize,pOutBuffer,&nOutBufferLen);	
	if(nOutBufferLen == 0 || NULL == pOutBuffer)
	{
		LOG_INFO("Parameter illegal!");
		return false;
	}
	bool bRet = GetTCPServiceModuleInstance()->SendData(m_pManageOwner,(UINT8*)pOutBuffer,nOutBufferLen,m_DeviceAddress);
	
	SafeDeleteArray(pOutBuffer);
	TRACE_OUT();
	return bRet;
}

void CDevice::ParserCallback(UINT32 wEvent, UINT32 wResultCode, UINT32 wDataLen,const char* pData,void *pOwner)
{

	TRACE_IN();
	CDevice* pDevice = (CDevice*)pOwner;
	if(WIS_CMD_LOGIN != wEvent && WIS_CMD_USER_AUTO_LOGIN != wEvent && WIS_CMD_HEART_BEAT != wEvent 
	   && WIS_CMD_USER_REGIST != wEvent && WIS_CMD_USER_RESET_PASSWORD != wEvent && WIS_CMD_USER_HEART_BEAT != wEvent)
	{
		if(!pDevice->IsLogined())
		{
			LOG_INFO("received something(event=%X) from a unloggined device and it`s not heart beat   --will drop it.  ",wEvent);
			return;
		}
	}

	//直接在设备层处理心跳。
	if(WIS_CMD_HEART_BEAT == wEvent || WIS_CMD_USER_HEART_BEAT == wEvent)
	{
		pDevice->m_isGetHeartBeatResponse = true;
		pDevice->m_loseHeartBeatTimes = 0;
		return;
	}

	DEVICE_INFO devInfo;  
	devInfo.bus_address = pDevice->m_DeviceAddress;

	devInfo.is_logined = pDevice->IsLogined(); 
	devInfo.login_type = pDevice->GetLoginType();
	devInfo.uuid = pDevice->GetUuid();
	
	CUniteDataModule::GetInstance()->ShowNetDataToInterface(devInfo, wEvent,wResultCode, wDataLen, pData);
	TRACE_OUT();
}

BUS_ADDRESS_POINTER CDevice::GetBusAddress(void)
{
	return m_DeviceAddress;
}

void CDevice::SetLogined(bool isLogined)
{
	std::lock_guard<std::mutex> lg(m_isLoginedMutex);
	m_isLogined = isLogined;
}

bool CDevice::IsLogined(void)
{
	std::lock_guard<std::mutex> lg(m_isLoginedMutex);
	return m_isLogined;
}

void CDevice::SetDeviceExpire(bool isExpire)
{
	std::lock_guard<std::mutex> lg(m_expireMutex);
	m_bExpire = isExpire;
}

bool CDevice::GetDeviceExpire()
{
	std::lock_guard<std::mutex> lg(m_expireMutex);
	return m_bExpire;
}


int CDevice::HeartBeratTimerHandler(void *device)
{
	TRACE_IN();
	CDevice *pDevice  = (CDevice *)device;
	
	if(pDevice->GetDeviceExpire()) 
		return -1;

	if(!pDevice->m_isGetHeartBeatResponse)
		pDevice->m_loseHeartBeatTimes++;
	
	if(pDevice->m_loseHeartBeatTimes >= 3)
	{
		LOG_INFO("lose heart beat %d times,will delete the link",pDevice->m_loseHeartBeatTimes);
		pDevice->m_pManageOwner->StopTcpServer(pDevice->m_DeviceAddress);  //this function will stop the thread.	and you can`t unregist the timer,otherwise is will case died lock.	
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
	if(this->m_DeviceAddress->model_type != TCP_SERVER_MODE)
	{
		LOG_INFO("heart beat sended only in server mode");
		return;
	}
	m_isGetHeartBeatResponse = false;
	m_loseHeartBeatTimes = 0;
	m_heartBeartTiemr.open(10,HeartBeratTimerHandler,this);

	TIMERMANAGER->Register(&m_heartBeartTiemr);
	
	TRACE_OUT();
}

void CDevice::StopHeartBeat()
{
	TIMERMANAGER->unRegister(&m_heartBeartTiemr);
}
