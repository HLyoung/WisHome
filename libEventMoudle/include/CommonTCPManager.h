
#ifndef _COMMONTCPMANAGER_H_
#define _COMMONTCPMANAGER_H_

#include "ICommonTCPManager.h"
#include "ISocketOwner.h"
#include "Tcpmodule_Config.h"
#include "ClientSocket.h"
#include "ServrSocket.h"
#include "EventBase.h"
#include "ISocket.h"
#include <event.h>
#include <list>
#include <pthread.h>

using namespace std;
class ClientSocket;
class DLL_DEFINE CommonTCPManager : public CICommonTCPManager
{
public:
    virtual ~CommonTCPManager(){}
	
    static CICommonTCPManager *getInstance();
	
	int BeginServer(const CADDRINFO &serAddr ,CISocketOwner *pSocketOwner);
	int ConnectServer(const CADDRINFO &serAddr,CISocketOwner *pSocketOwner, int bLongConnect = 0,int nTimeOut = 120);
	int SetHeartBeatData(void* dwHandle,const char *pData,int nlen){}
	int SetHeartBeatInterval(void* dwHandle,int nInterval){}
	int SetConnectTimeOut(void* dwHandle,int nTime){}
	int CloseConnection(void* dwHandle);
	int Send(void*,const char * pData,int nProLen);	
	
	list<ClientSocket *> ClientSocketList;
	list<ServrSocket *> ServrSocketList;
	std::mutex clientMutex;
	std::mutex servrMutex;


private:
    CommonTCPManager();
	static CICommonTCPManager* _instance;
	static std::mutex _insMutex;

};


CICommonTCPManager *GetCommonTCPManager(void);



#endif 
