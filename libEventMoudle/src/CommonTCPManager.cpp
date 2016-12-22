

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
	std::lock_guard<std::mutex> lg(servrMutex);
	std::list<ServrSocket *>::iterator ite = ServrSocketList.begin();
	for(; ite != ServrSocketList.end(); ite++)
		if((*ite)->bufMap.find((struct bufferevent *)handle) != (*ite)->bufMap.end())
			return bufferevent_write((struct bufferevent*)handle,pData,nProLen);

	std::lock_guard<std::mutex> cg(clientMutex);
	std::list<ClientSocket*>::iterator ote = ClientSocketList.begin();
	for(; ote != ClientSocketList.end(); ote++)
		if((*ote)->_bev == (struct bufferevent*)handle)
			return bufferevent_write((struct bufferevent*)handle,pData,nProLen);

	return -1;

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
		//	(*sIte)->closeServer((struct bufferevent *)dwHandle);
			return 0;
		}
	}
   TRACE_OUT();
   return -1;
}


CICommonTCPManager *GetCommonTCPManager()
{
	return CommonTCPManager::getInstance();
}





