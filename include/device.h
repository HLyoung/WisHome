

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "transfer_structs.h"
#include "deviceManager.h"
#include "IParseProtocolModule.h"
#include "macro.h"
#include "libLog.h"
#include "wis.h"
#include "UniteDataModule.h"
#include "TimerThread.h"

class CDeviceManager;


class CDevice
{
public:
	CDevice();
	CDevice(CDeviceManager *pOwner,BUS_ADDRESS_POINTER   address);
	virtual ~CDevice();
	
	bool Receive(UINT32 length,void *data);
	bool Send(INT32 nCmd,void *data,INT32 nDataLength);
	BUS_ADDRESS_POINTER GetBusAddress(void);
	void SetLogined(bool logged);
	bool IsLogined(void);
	
	int GetLoginType(void){return m_loginType;}
	void SetLoginType(int type){m_loginType = type;}
	
	std::string GetUuid(void){return m_uuid;}
	void SetUuid(std::string uuid){m_uuid = uuid;}

	void  SetDeviceExpire(bool isExpire);
	bool GetDeviceExpire();
	
	
	
private:
	static void ParserCallback(UINT32 wEvent,UINT32 wResultCode,UINT32 \
	wDataLen,const char *pData,void *pOwner);
	
private:
	CDeviceManager *m_pManageOwner;
	BUS_ADDRESS_POINTER m_DeviceAddress;
	bool m_bExpire;
	HANDLE m_pProtocolPaser;
	bool m_isLogined;
	char m_loginType;
	std::string m_uuid;
	std::mutex m_expireMutex;
	std::mutex m_isLoginedMutex;

};




#endif