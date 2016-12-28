

#include "CommonTCPManager.h"

CICommonTCPManager* CommonTCPManager::_instance = NULL;
std::mutex CommonTCPManager::_insMutex;


CommonTCPManager::CommonTCPManager()
{	
	tPool = new CThreadPool(20);
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
   	ServrSocket *pServ = new ServrSocket(serAddr,pSocketOwner,this);
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
		if((*ite)->bufSend((struct bufferevent *)handle,pData,nProLen) > 0)
			return 0;
	return -1;

}

int CommonTCPManager::CloseConnection(void * dwHandle)
{
   return -1;
}


CICommonTCPManager *GetCommonTCPManager()
{
	return CommonTCPManager::getInstance();
}


void CommonTCPManager::AddJobToPool(CJob * job,unsigned int index)
{
	tPool->Run(job,index);
}





