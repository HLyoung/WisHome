

#ifndef _DEVICE_MANAGER_H_
#define _DEVICE_MANAGER_H_

#include  <map>
#include <string>
#include <macro.h>
#include <mutex>
#include "device.h"
#include "transfer_structs.h"
#include "ITCPServiceModule.h"
#include "UniteDataModule.h"

class CDevice;
class CDeviceManager
{
public:
	static CDeviceManager *GetInstance();
	bool StartTcpServer(int nPort);
	bool StopTcpServer(BUS_ADDRESS_POINTER pBusAddress);
	bool CreateConnectToDevice(string &strIp,int nPort);
	bool SendData(BUS_ADDRESS_POINTER busAddress,int nRole,int nDataType,char *pData,int nDataSize);
	bool ScanAllDevice();
	
protected:
	virtual void OnConnect(UINT32 size,void *data);
	virtual void OnDisconnect(UINT32 size,void *data);
	virtual void OnReceive(UINT32 size,void *data);
	virtual void OnSend(UINT32 size,void *data);
	
	
private:
	CDeviceManager(void);
	~CDeviceManager(void);
	static void MessageCallback(HANDLE owner,UINT32 message,UINT32 message_size,void *message_data);
	string GetAddressKey(BUS_ADDRESS_POINTER address);
	CDevice *GetDeviceClient(BUS_ADDRESS_POINTER address);
	int GetTcpServicePort();
	void StartClearTimer();
	
	
	static int ClearDeviceTimerHandler(void *manager);
	
	
private:
	static CDeviceManager *m_pInstance;
	INITIAL_ARGUMENTS m_tInitialArguments;
	map<string,CDevice*> m_mapDevice;
	std::mutex m_Device_mutex;
	Timer m_tClearExpireDevice;
	
};






#endif