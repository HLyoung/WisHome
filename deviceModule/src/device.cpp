

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
}


CDevice::~CDevice()
{
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
//�����ע��������豸ע��״̬��Ϊ�Ѿ�ע�ᡣ
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
	if(WIS_CMD_SERVICE_KICKOUT_USER == nCmd)   //����û����߳���ֱ�ӽ�����Ϊδע��״̬��
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
	   && WIS_CMD_USER_REGIST != wEvent && WIS_CMD_USER_RESET_PASSWORD != wEvent && WIS_CMD_USER_HEART_BEAT != wEvent 
	   && !pDevice->IsLogined()){
		LOG_INFO("received something(event=%X) from a unloggined device and it`s not heart beat   --will drop it.  ",wEvent);
		return;
		}

	//����ֱ�������� ��Ϊ�Ѿ���socket�㴦������
	if(WIS_CMD_HEART_BEAT == wEvent || WIS_CMD_USER_HEART_BEAT == wEvent)
		return;

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
