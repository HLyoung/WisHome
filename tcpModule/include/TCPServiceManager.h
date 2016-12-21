

#ifndef _TCP_SERVICE_MANAGER_H_
#define _TCP_SERVICE_MANAGER_H_

#include "TCPService.h"
#include "macro.h"
#include "ISocketOwner.h"
#include "transfer_structs.h"
#include "libLog.h"
#include "tools.h"
#include <map>
#include <list>

#include "Thread.h"

using namespace std;

class CTCPService;
class CTCPServiceManage
{
public:
	CTCPServiceManage(void);
	virtual ~CTCPServiceManage(void);
	
	static CTCPServiceManage *GetInstance();
	
	CTCPService *StartServer(void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback);
	CTCPService *CreateTcpClient(void* owner,string &strIp,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback);
	
	void StopService(BUS_ADDRESS_POINTER pBusAddress);
	
	void NotifyMessageConnect(CTCPService *pTcpService,void* dwHandle,BUS_ADDRESS_POINTER pBus_address);
	void NotifyMessageDisconnect(CTCPService *pTcpService,BUS_ADDRESS_POINTER pBus_address);
	void NotifyMessageReceiveData(CTCPService *pTcpService,void *handle,const char *pData,int nLen,BUS_ADDRESS_POINTER pBus_address);
	
	bool SendData(void * handle,UINT8 *pData,UINT32 length,BUS_ADDRESS_POINTER pBus_address);
	
	void RemoveTcpService(BUS_ADDRESS_POINTER pBus_address);


private:
    void GetLinkAddressInfo(CTCPService *pTcpService,HOST_ADDRESS& address, BUS_ADDRESS_POINTER pBus_address);
	void SaveLinkAndCallBack(BUS_ADDRESS_POINTER pBus_address,CTCPService *pTcpService,void *handle);
	void DeleteLinkAndCallBack(BUS_ADDRESS_POINTER pBus_address,CTCPService* pTcpService);
	void Release();
	std::string tcpGetAddressKey(BUS_ADDRESS_POINTER pAddress);
	
private:
	static CTCPServiceManage *m_instance;
	std::map<string,CTCPService*> m_mapTcpLink;
	std::mutex m_tcpLinkMutex;
};





#endif