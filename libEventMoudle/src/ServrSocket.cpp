

#include "ServrSocket.h"
#include "macro.h"
CISocketOwner * ServrSocket::owner = NULL; 


ServrSocket::ServrSocket(const CADDRINFO &sveAddr,CISocketOwner *servr_owner)
{
	m_LocalAddr = sveAddr;
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
        SafeDelete(pSock);
		pthread_exit(0);
	}

    evutil_socket_t  fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        LOG_ERROR("create socket failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    if(evutil_make_socket_nonblocking(fd) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket no block failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    if(evutil_make_socket_closeonexec(fd) < 0 ){
        evutil_closesocket(fd);
        LOG_ERROR("make socket close on exec failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    if(evutil_make_listen_socket_reuseable(fd) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket reuseable failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    int on = 1;
    if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&on,sizeof(on)) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket keep alieve failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }
    int nodelay_on = 1;
    if(setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(void*)&nodelay_on,sizeof(nodelay_on)) < 0){
        evutil_closesocket(fd);
        LOG_ERROR("make socket no delay failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }
   
    int sendbuf = 0;
    if(setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(void*)&sendbuf,sizeof(sendbuf)) < 0)
    {
        evutil_closesocket(fd);
        LOG_ERROR("make socket send buf 0 failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    if(bind(fd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
    {
        LOG_ERROR("bind sock failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }
    evthread_use_pthreads();  //this is very important,otherwise you can`t operator base in another thread......
    
	struct event_base *base = event_base_new();
    struct evconnlistener* listener = evconnlistener_new(base,accept_conn_cb,pSock,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,100,fd);
	if(!listener)
	{
		LOG_ERROR("Start Server(ip = %s,port =%d) failed",pSock->getLocalAddr().GetIP(),pSock->getLocalAddr().GetPort());
		SafeDelete(pSock);
		event_base_free(base);
		pthread_exit(0);
	}

    evconnlistener_set_error_cb(listener,accept_error_cb);
	
	LOG_INFO("server is started  ip = %s,port = %d",pSock->getLocalAddr().GetIP(),pSock->getLocalAddr().GetPort());
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
	evutil_socket_t fd;
	void *address;
	void *pSock;
}Signel_connect_param,*pSignel_connect_param;

typedef struct{
	ServrSocket*pSock;
	struct bufferevent  *bev;
	BUS_ADDRESS_POINTER pAddress;
	struct event_base * base;
}Timer_cb_param,*pTimer_cb_param;

void ServrSocket::accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *address, \
		                int socklen, void *ctx)
{
	TRACE_IN();  	
	pSignel_connect_param param = new Signel_connect_param;
	param->fd = fd;
	param->address = address;
	param->pSock = ctx;
	
	pthread_t thread;
	pthread_create(&thread,NULL,handle_signel_connct,param);
	pthread_detach(thread);
	
	TRACE_OUT();
}

void *ServrSocket::handle_signel_connct(void *p)
{
	TRACE_IN();
	pSignel_connect_param param = (pSignel_connect_param)p;
	ServrSocket *pSock  = (ServrSocket*)param->pSock;
	
	evutil_make_socket_nonblocking(param->fd);
	struct event_base *base = event_base_new();
	struct bufferevent *bev = bufferevent_socket_new(base,param->fd,BEV_OPT_CLOSE_ON_FREE);   //will close the socket when free bufferevent.

	BUS_ADDRESS_POINTER pBus_address = new BUS_ADDRESS;
	pBus_address->size = sizeof(BUS_ADDRESS);
	pBus_address->model_type = TCP_SERVER_MODE;
	pBus_address->bus_address_type = BUS_ADDRESS_TYPE_TCP;
	pBus_address->host_address.size = sizeof(HOST_ADDRESS);
	pBus_address->host_address.port = ntohs(((sockaddr_in*)(param->address))->sin_port);
	memcpy(pBus_address->host_address.ip,inet_ntoa(((sockaddr_in*)(param->address))->sin_addr),HOST_NAME_LENGTH);
	param->address = pBus_address;

	bufferevent_setwatermark(bev,EV_READ,0,0);
    bufferevent_setwatermark(bev,EV_WRITE,0,0);
	bufferevent_setcb(bev,pSock->read_cb,pSock->write_cb,pSock->event_cb,param);
	bufferevent_enable(bev,EV_READ|EV_WRITE);

	pTimer_cb_param tParam = new Timer_cb_param;
	tParam->pSock = (ServrSocket *)param->pSock;
	tParam->bev = bev;
	tParam->pAddress = pBus_address;
	tParam->base = base;
	struct timeval tv = {HEARTBEAT_TIME,0};         //heartbeat timer.
	struct event *timeout = event_new(base, -1, EV_PERSIST, timer_cb, tParam);		
	evtimer_add(timeout, &tv);

	pSock->loseHeartBeatCount[bev] = 0;        //count lose heart beat times.
	pSock->bufMapAddBuf(bev,pBus_address);
	pSock->owner->OnAccept((void*)bev,pBus_address);			
	event_base_dispatch(base);  
	
    event_base_free(base);	
	SafeDelete(pBus_address);
	SafeDelete(param);
	SafeDelete(tParam);
	TRACE_OUT();
	
}


void ServrSocket::read_cb(struct bufferevent * bev,void * ctx)
{
	TRACE_IN();
	pSignel_connect_param param = (pSignel_connect_param)ctx;
	((ServrSocket *)(param->pSock))->loseHeartBeatCount[bev] = 0;    //no matter recveied anything.set lose heart beat time to 0;
	
	struct evbuffer* readbuffer = bufferevent_get_input(bev);
	int len = evbuffer_get_length(readbuffer);
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(((pSignel_connect_param)ctx)->address);
    owner->OnCmdCome((void*)bev,(const char *)evbuffer_pullup(readbuffer,-1),len,pBus_address);
	evbuffer_drain(readbuffer,len);
	TRACE_OUT();
}


void ServrSocket::write_cb(struct bufferevent * bev,void * ctx)
{


	
}

void ServrSocket::accept_error_cb(struct evconnlistener * listener,void * ctx)
{
    TRACE_IN();
    LOG_ERROR("accept error happens");
    TRACE_OUT();
}

void ServrSocket::event_cb(struct bufferevent * bev,short events,void * ctx)
{
	TRACE_IN();
	evutil_socket_t fd = bufferevent_getfd(bev);
	BUS_ADDRESS_POINTER pBus_address = (BUS_ADDRESS_POINTER)(((pSignel_connect_param)ctx)->address);
	ServrSocket *pSock  = (ServrSocket*)(((pSignel_connect_param)ctx)->pSock);
	
	if(events & (BEV_EVENT_ERROR))
		{				 
            int errorcode =  evutil_socket_geterror(fd);
            LOG_ERROR("error(event = %d) happens on socket(fd = %d), error message: %s",(int)events,(int)fd,evutil_socket_error_to_string(errorcode));
            owner->OnClose((void*)bev,pBus_address);
			pSock->bufMapDeleteBuf(bev);
		}
	else if(events & (BEV_EVENT_EOF))
		{		
			LOG_INFO("EOF readed on socket(fd = %d)",(int)fd);
			owner->OnClose((void*)bev,pBus_address);
			pSock->bufMapDeleteBuf(bev);
		}
	else
		LOG_INFO("something unknow happens(event = %d ,fd = %d)",(int)events,(int)fd);	
	TRACE_OUT();
}


void ServrSocket::timer_cb(int fd,short event,void *ctx)
{
	pTimer_cb_param param = (pTimer_cb_param)ctx;
	ServrSocket *pSock = param->pSock;
	if(pSock->loseHeartBeatCount.find(param->bev) != pSock->loseHeartBeatCount.end()){
		pSock->loseHeartBeatCount[param->bev] ++;

		if(pSock->loseHeartBeatCount[param->bev] > 4){
			LOG_ERROR("delete link by lose heart beat too many times!!");
			owner->OnClose(param->bev,param->pAddress);
			pSock->bufMapDeleteBuf(param->bev);
			event_base_loopbreak(param->base);
			return;
			}

		time_t seconds = time((time_t*)NULL);
		char buf[16] = {0};
		*((unsigned int*)(buf + 4)) = WIS_CMD_HEART_BEAT;
		*((unsigned int*)(buf + 8)) = 4;
		*((unsigned int*)(buf + 12)) = (unsigned int)seconds;
		*((unsigned int*)(buf)) = (unsigned int)(WIS_CMD_HEART_BEAT + 4 + seconds);
		bufferevent_write(param->bev,buf,16);
		}
	
}
void ServrSocket::bufMapAddBuf(struct bufferevent *bev,BUS_ADDRESS_POINTER pBusAddress)
{
	std::lock_guard<std::mutex> lg(bufMapMutex);
	bufMap.insert(pair<struct bufferevent*,BUS_ADDRESS_POINTER>(bev,pBusAddress));
}

bool ServrSocket::bufMapDeleteBuf(struct bufferevent * bev)
{
	TRACE_IN();
	std::lock_guard<std::mutex> lg(bufMapMutex);
	std::map<struct bufferevent*,BUS_ADDRESS_POINTER>::iterator ite = bufMap.find(bev);
	if(ite != bufMap.end())
	{
		loseHeartBeatCount.erase(bev);
		bufMap.erase(bev);
		bufferevent_free(bev);
		return true;
	}

	TRACE_OUT();
	return false;
}
void ServrSocket::closeServer(struct bufferevent*bev)
{
	TRACE_IN();
	bufMapDeleteBuf(bev);
	TRACE_OUT();
}





