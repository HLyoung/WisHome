

#ifndef _ITCPSERVICE_MODULE_H
#define _ITCPSERVICE_MODULE_H

#include <string>
#include "transfer_structs.h"
#include "macro.h"

using namespace std;
class ICTCPServiceModule
{
public:
	virtual bool StartTCPServer(void* owner, int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback) = 0;
	virtual bool ConnectGatewayServer(void* owner, string& strIp, int nPort, BUS_SERVICE_MESSAGE_CALLBACK callback) = 0;
	virtual bool StopService(BUS_ADDRESS_POINTER pBusAddress) = 0;
	virtual bool SendData(void* hHandle, UINT8* pData, UINT32 length, BUS_ADDRESS_POINTER send_argument) = 0;
};

ICTCPServiceModule *GetTCPServiceModuleInstance();




#endif