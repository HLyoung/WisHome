

#include "ServrSocket.h"
#include "macro.h"
#include "netJob.h"
CISocketOwner * ServrSocket::owner = NULL; 


ServrSocket::ServrSocket(const CADDRINFO &sveAddr,CISocketOwner *servr_owner,CommonTCPManager *_manager):manager(_manager),m_LocalAddr(sveAddr)
{
	owner = servr_owner;
}


void *ServrSocket::startServrThread(void *p)
{
	TRACE_IN();
	ServrSocket * pSock = (ServrSocket *)p;

	struct sockaddr_in sin;
	memset(&sin,0,sizeof(sin));
	sin.sin_family = AF_INET;   
	sin.sin_port = htons(pSock->getLocalAddr().GetPort());
	int tmp = inet_aton(pSock->getLocalAddr().GetIP(),&sin.sin_addr);
	if(0 == tmp)
	{
		LOG_ERROR("address convert failed. ip = %s,port = %d",pSock->getLocalAddr().GetIP(),pSock->getLocalAddr().GetPort());
		pthread_exit(0);
	}

    evutil_socket_t  fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        LOG_ERROR("create socket failed");
        pthread_exit(0);
    }

    if(evutil_make_socket_nonblocking(fd) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket no block failed");
        pthread_exit(0);
    }

    if(evutil_make_socket_closeonexec(fd) < 0 ){
        evutil_closesocket(fd);
        LOG_ERROR("make socket close on exec failed");
        pthread_exit(0);
    }

    if(evutil_make_listen_socket_reuseable(fd) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket reuseable failed");
        pthread_exit(0);
    }

    int on = 1;
    if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&on,sizeof(on)) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket keep alieve failed");
        pthread_exit(0);
    }
    int nodelay_on = 1;
    if(setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(void*)&nodelay_on,sizeof(nodelay_on)) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket no delay failed");
        pthread_exit(0);
    }
   
    int sendbuf = 0;
    if(setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(void*)&sendbuf,sizeof(sendbuf)) < 0)
    {
        evutil_closesocket(fd);
        LOG_ERROR("make socket send buf 0 failed");
        pthread_exit(0);
    }

    if(bind(fd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
    {
        LOG_ERROR("bind sock failed");
        pthread_exit(0);
    }
    evthread_use_pthreads();  //this is very important,otherwise you can`t operator base in another thread......
    
	struct event_base *base = event_base_new();
    struct evconnlistener* listener = evconnlistener_new(base,accept_conn_cb,pSock,\
		LEV_OPT_THREADSAFE|LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE|LEV_OPT_LEAVE_SOCKETS_BLOCKING,100,fd);
	if(!listener){
		LOG_ERROR("SERVER STARTED FAILED");
		event_base_free(base);
		pthread_exit(0);
		}

    evconnlistener_set_error_cb(listener,accept_error_cb);
	
	LOG_INFO("SERVER STARTED SUCCESS: ip = %s,port = %d !",pSock->getLocalAddr().GetIP(),pSock->getLocalAddr().GetPort());
	event_base_dispatch(base);	

	evconnlistener_free(listener);
	event_base_free(base);
	TRACE_OUT();
}

int ServrSocket::startServr()
{
	TRACE_IN();
	pthread_t thread;
	pthread_create(&thread,NULL,startServrThread,this);
	pthread_detach(thread);
	TRACE_OUT();
}
ServrSocket::~ServrSocket()
{
}

typedef struct{
	void *address;
	void *pSock;
	void *bev;
}Signel_connect_param,*pSignel_connect_param;

void ServrSocket::accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *address, \
		                int socklen, void *ctx)
{
	TRACE_IN();  	
	ServrSocket *pSock = (ServrSocket *)ctx;
	BUS_ADDRESS_POINTER pBus_address = new BUS_ADDRESS;
	pBus_address->size = sizeof(BUS_ADDRESS);
	pBus_address->model_type = TCP_SERVER_MODE;
	pBus_address->bus_address_type = BUS_ADDRESS_TYPE_TCP;
	pBus_address->host_address.size = sizeof(HOST_ADDRESS);
	pBus_address->host_address.port = ntohs(((sockaddr_in*)(address))->sin_port);
	memcpy(pBus_address->host_address.ip,inet_ntoa(((sockaddr_in*)(address))->sin_addr),HOST_NAME_LENGTH);

	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);

	pSignel_connect_param param = new Signel_connect_param;
	param->address = (void*)pBus_address;
	param->pSock = (void *)ctx;
	param->bev = (void *)bev;
	bufferevent_setcb(bev,read_cb,NULL,event_cb,param);
	bufferevent_enable(bev,EV_READ|EV_WRITE);

	pSock->mCounter[bev] = 0;  
	struct timeval tv = {HEARTBEAT_TIME,0};         //heartbeat timer.
	struct event *timeout = event_new(base, -1, EV_PERSIST, timer_cb, param);
	evtimer_add(timeout, &tv);
	pSock->bufMapAddBuf(bev,timeout);

	jobAccept *job = new jobAccept(bev,pBus_address,pSock->getOwner());
	pSock->getManager()->AddJobToPool(job,(unsigned int)fd);
	TRACE_OUT();
}



void ServrSocket::read_cb(struct bufferevent * bev,void * ctx)
{
	TRACE_IN();
	pSignel_connect_param param = (pSignel_connect_param)ctx;
	ServrSocket *pSocket =  (ServrSocket *)(param->pSock);
	pSocket->mCounter[bev] = 0;       //no matter recveied what.set lose heart beat time to 0;
	
	struct evbuffer* readbuffer = bufferevent_get_input(bev);
	int len = evbuffer_get_length(readbuffer);
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(((pSignel_connect_param)ctx)->address);

	CJob *job = new jobRead(bev,pBus_address,pSocket->getOwner(),len,(char *)evbuffer_pullup(readbuffer,-1));	
	pSocket->getManager()->AddJobToPool(job,(unsigned int )bufferevent_getfd(bev));
	evbuffer_drain(readbuffer,len);
	TRACE_OUT();
}


void ServrSocket::write_cb(struct bufferevent * bev,void * ctx)
{	
}

void ServrSocket::accept_error_cb(struct evconnlistener * listener,void * ctx)
{
    TRACE_IN();
    struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	LOG_ERROR("ERROR HAPPENS(err= %d) ON LISTENER. errMsg: %s",err,evutil_socket_error_to_string(err));
	event_base_loopexit(base,NULL);
    TRACE_OUT();
}

void ServrSocket::event_cb(struct bufferevent * bev,short events,void * ctx)
{
	TRACE_IN();
	evutil_socket_t fd = bufferevent_getfd(bev);
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(((pSignel_connect_param)ctx)->address);
	ServrSocket *pSock  = (ServrSocket*)(((pSignel_connect_param)ctx)->pSock);
	
	if(events & (BEV_EVENT_ERROR)){			 
            int errorcode =  evutil_socket_geterror(fd);
            LOG_ERROR("ERROR HAPPENS: ip=%s,port=%u,event=%d,error message: %s",pBus_address->host_address.ip,pBus_address->host_address.port,(int)events,evutil_socket_error_to_string(errorcode));
			pSock->bufMapDeleteBuf(ctx);
		}
	else if(events & (BEV_EVENT_EOF)){		
			LOG_INFO("EOF READED: ip=%s,port=%u",pBus_address->host_address.ip,pBus_address->host_address.port);
			pSock->bufMapDeleteBuf(ctx);
		}
	else
		LOG_INFO("UNKNOW HAPPENS: ip=%s,port=%u,event = %d)",pBus_address->host_address.ip,pBus_address->host_address.port,(int)events);	
	TRACE_OUT();
}


void ServrSocket::timer_cb(int fd,short event,void *ctx)
{
	pSignel_connect_param  param = (pSignel_connect_param)ctx;
	ServrSocket *pSock =(ServrSocket *)(param->pSock);
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(param->address);
	struct bufferevent * bev = (struct bufferevent *)(param->bev);

	std::map<struct bufferevent*,int>::iterator ite =pSock->mCounter.find(bev);
	if(ite != pSock->mCounter.end()){
		pSock->mCounter[bev] ++;
		if(pSock->mCounter[bev] > 4){
			LOG_INFO("LINK EXPRESS: ip=%s,port=%u",pBus_address->host_address.ip,pBus_address->host_address.port);
			pSock->bufMapDeleteBuf(ctx);
			return;
		}
		//time_t seconds = time((time_t*)NULL);
		unsigned char buf[16] = {0x80,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		//*((unsigned int*)(buf + 4)) = WIS_CMD_HEART_BEAT;
		//*((unsigned int*)(buf + 8)) = 4;
		//*((unsigned int*)(buf + 12)) = (unsigned int)seconds;
		//unsigned checkSum = 0;
		//for(int i = 0;i<12;i++)
		//checkSum += *((unsigned char *)(buf + 4 + i));
	    //*((unsigned int*)(buf)) = checkSum;
	    pSock->bufSend(bev,(char *)buf,16);
	}
}


void ServrSocket::bufMapAddBuf(struct bufferevent *bev,struct event* timeout)
{
	std::lock_guard<std::mutex> lg(bufMapMutex);
	bufMap.insert(pair<struct bufferevent*,struct event *>(bev,timeout));
}

bool ServrSocket::bufMapDeleteBuf(void *ctx)
{
	TRACE_IN();
	pSignel_connect_param param = (pSignel_connect_param)ctx;
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(param->address);
	struct bufferevent *bev = (struct bufferevent *)(param->bev);
	
	std::lock_guard<std::mutex> lg(bufMapMutex);
	std::map<struct bufferevent*,struct event *>::iterator ite = bufMap.find(bev);
	if(ite != bufMap.end()){
		CJob *job = new jobClose(pBus_address,owner);
		manager->AddJobToPool(job,bufferevent_getfd(bev));	
		std::map<struct bufferevent*,int>::iterator ote= mCounter.find(bev);
		if(ote != mCounter.end())
			mCounter.erase(bev);

		evtimer_del((struct event*)(ite->second));
		bufMap.erase(bev);
		bufferevent_free(bev);
		return true;
		}
	TRACE_OUT();
	return false;
}


int ServrSocket::bufSend(struct bufferevent* bev,const char *data,int dataLen)
{
	//std::lock_guard<std::mutex> lb(bufMapMutex);
	//std::map<struct bufferevent*,struct event*>::iterator ite = bufMap.find(bev);
	//if(ite != bufMap.end())
		return bufferevent_write(bev,data,dataLen);
	//return -1;
}


