

#ifndef _TCPSERVICEMODULE_H_
#define _TCPSERVICEMODULE_H_

#include "TCPServiceManager.h"
#include "macro.h"
#include "libLog.h"
#include "ITCPServiceModule.h"

class CTCPServiceModule: public ICTCPServiceModule
{
public:
	static CTCPServiceModule *GetInstance();
	virtual bool StartTCPServer(void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback);
	virtual bool ConnectGatewayServer(void* owner, string& strIp, int nPort, BUS_SERVICE_MESSAGE_CALLBACK callback);
	virtual bool StopService(BUS_ADDRESS_POINTER pBusAddr);
	virtual bool SendData(void* hHandle, UINT8* pData, UINT32 length, BUS_ADDRESS_POINTER send_argument);          
	
private:
	CTCPServiceModule();
	virtual ~CTCPServiceModule();
	
private:
	static CTCPServiceModule *m_pInstance;	
};
#endif