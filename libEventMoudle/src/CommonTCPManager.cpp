

#include "CommonTCPManager.h"

CICommonTCPManager* CommonTCPManager::_instance = NULL;
std::mutex CommonTCPManager::_insMutex;


CommonTCPManager::CommonTCPManager()
{	
}

CICommonTCPManager * CommonTCPManager::getInstance()
{
	_insMutex.lock();
	if(NULL == _instance)
	{
		_instance = new CommonTCPManager();
	}
	_insMutex.unlock();
	return _instance;
}

int CommonTCPManager::BeginServer(const CADDRINFO &serAddr,CISocketOwner *pSocketOwner)
{
	TRACE_IN();
   	ServrSocket *pServ = new ServrSocket(serAddr,pSocketOwner);
	pServ->startServr();

	servrMutex.lock();
	ServrSocketList.push_back(pServ);
	servrMutex.unlock();
	TRACE_OUT();
	return 1;
}
int CommonTCPManager::ConnectServer(const CADDRINFO & serAddr,CISocketOwner * pSocketOwner,int bLongConnect,int nTimeOut)
{   
	ClientSocket *pClient = new ClientSocket(serAddr,pSocketOwner,this);
	pClient->startClient();

	clientMutex.lock();
	ClientSocketList.push_back(pClient);
	clientMutex.unlock();
    return 1;
}

int CommonTCPManager::Send(void * handle,const char * pData,int nProLen)
{
     return bufferevent_write((struct bufferevent*)handle,pData,nProLen);

}

int CommonTCPManager::CloseConnection(void * dwHandle)
{
	TRACE_IN();
	std::list<ClientSocket*>::iterator ite = ClientSocketList.begin();
	for(; ite != ClientSocketList.end();ite++){
		if((*ite)->_bev == (struct bufferevent *)dwHandle){
			(*ite)->closeClient();
			return 0;
		}
	}
	std::list<ServrSocket*>::iterator sIte = ServrSocketList.begin();
	for(; sIte != ServrSocketList.end();sIte++){
		if((*sIte)->bufMap.find((struct bufferevent *)dwHandle) != (*sIte)->bufMap.end()){
			(*sIte)->bufMapDeleteBuf((struct bufferevent *)dwHandle);
			return 0;
		}
	}
   if(sIte == ServrSocketList.end())
   LOG_INFO("the link about to delete not found");
   TRACE_OUT();
	return -1;
}


CICommonTCPManager *GetCommonTCPManager()
{
	return CommonTCPManager::getInstance();
}





