

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
	CDevice(CDeviceManager *pOwner,BUS_ADDRESS address);
	virtual ~CDevice();
	
	bool Receive(UINT32 length,void *data);
	bool Send(INT32 nCmd,void *data,INT32 nDataLength);
	void GetBusAddress(BUS_ADDRESS & tBusAddress);
	void UpdateAddressInfo(BUS_ADDRESS &tBusAddress);
	void SetLogined(bool logged);
	bool IsLogined(void);
	
	int GetLoginType(void){return m_loginType;}
	void SetLoginType(int type){m_loginType = type;}
	
	std::string GetUuid(void){return m_uuid;}
	void SetUuid(std::string uuid){m_uuid = uuid;}
	void StartHeartBeat();
	void StopHeartBeat();
	static int HeartBeratTimerHandler(void*);
	
	
	
private:
	static void ParserCallback(UINT32 wEvent,UINT32 wResultCode,UINT32 \
	wDataLen,const char *pData,void *pOwner);
	
private:
	CDeviceManager *m_pManageOwner;
	BUS_ADDRESS m_DeviceAddress;
	bool m_bExpire;
	HANDLE m_pProtocolPaser;
	bool m_isLogined;
	char m_loginType;
	std::string m_uuid;
	Timer m_heartBeartTiemr;
	bool m_isGetHeartBeatResponse;
	unsigned int m_loseHeartBeatTimes;

};




#endif