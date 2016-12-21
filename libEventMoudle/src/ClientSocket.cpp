

#include "ClientSocket.h"
#include "CommonTCPManager.h"


CISocketOwner * ClientSocket::owner = NULL;

ClientSocket::ClientSocket(const CADDRINFO &srvAddr,const CISocketOwner *client_owner,CommonTCPManager *manager)
{
	
    m_RemoteAddr = srvAddr;
	owner = (CISocketOwner *)client_owner; 
	manager_ = manager;
		
}

void *ClientSocket::startClientThread(void *p)
{

	TRACE_IN();
	
	ClientSocket *pSock = (ClientSocket *)p;
	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(pSock->getRemoAddr().GetPort());
	int ret= inet_aton(pSock->getRemoAddr().GetIP(),&server_addr.sin_addr);
	if(0 == ret)
	{
		LOG_ERROR("IP convet failed ip = %s,port = %d ",pSock->getRemoAddr().GetIP(),pSock->getRemoAddr().GetPort());
		SafeDelete(pSock);
		exit(1);
	}
	
	int iSockFd = socket(PF_INET,SOCK_STREAM,0);
	if(-1 == iSockFd)
	{
		LOG_ERROR("create socket failed iSocket = %d",iSockFd); 
		SafeDelete(pSock);
	    pthread_exit(0);
	}
	
	 if(evutil_make_socket_nonblocking(iSockFd) < 0){
        evutil_closesocket(iSockFd);
        LOG_ERROR("make socket no block failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }

    if(evutil_make_socket_closeonexec(iSockFd) < 0 ){
        evutil_closesocket(iSockFd);
        LOG_ERROR("make socket close on exec failed");
        SafeDelete(pSock);
        pthread_exit(0);
    }
	int on = 1;
	if(setsockopt(iSockFd,SOL_SOCKET,SO_KEEPALIVE,(void*)&on,sizeof(on)) < 0){
		   evutil_closesocket(iSockFd);
		   LOG_ERROR("make socket keep alieve failed");
		   SafeDelete(pSock);
		   pthread_exit(0);
	   }
	 int nodelay_on = 1;
	 if(setsockopt(iSockFd,IPPROTO_TCP,TCP_NODELAY,(void*)&nodelay_on,sizeof(nodelay_on)) < 0){
		  evutil_closesocket(iSockFd);
		  LOG_ERROR("make socket no delay failed");
		  SafeDelete(pSock);
		  pthread_exit(0);
	  }
	  
	  int sendbuf = 0;
	  if(setsockopt(iSockFd,SOL_SOCKET,SO_SNDBUF,(void*)&sendbuf,sizeof(sendbuf)) < 0)
	  {
		  evutil_closesocket(iSockFd);
		  LOG_ERROR("make socket send buf 0 failed");
		  SafeDelete(pSock);
		  pthread_exit(0);
	}

	struct event_base *base = event_base_new();
	struct bufferevent * bev = bufferevent_socket_new(base,iSockFd,BEV_OPT_CLOSE_ON_FREE);

	pSock->_bev = bev;
	BUS_ADDRESS_POINTER pBus_address = new BUS_ADDRESS;
	pBus_address->size = sizeof(BUS_ADDRESS);
	pBus_address->model_type = TCP_CLIENT_MODE;
	pBus_address->bus_address_type = BUS_ADDRESS_TYPE_TCP;
	pBus_address->host_address.size = sizeof(HOST_ADDRESS);
	pBus_address->host_address.port = pSock->getRemoAddr().GetPort();
	memcpy(pBus_address->host_address.ip,pSock->getRemoAddr().GetIP(),HOST_NAME_LENGTH);
	
	bufferevent_setcb(bev,read_cb,NULL,event_cb,pBus_address);
	bufferevent_enable(bev,EV_READ|EV_WRITE);
	
	if(bufferevent_socket_connect(bev,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
	{
		LOG_ERROR("connect server failed");
		bufferevent_free(bev);
		SafeDelete(pSock);
		pthread_exit(0);			
	}
    
	event_base_dispatch(base);

	pSock->owner->OnClose(pBus_address);
	SafeDelete(pSock);
	SafeDelete(pBus_address);
	TRACE_OUT();
}

int ClientSocket::startClient()
{
	TRACE_IN();
	pthread_t thread;
	pthread_create(&thread,NULL,startClientThread,this);
	pthread_detach(thread);
	TRACE_OUT();
}


void ClientSocket::read_cb(struct bufferevent * bev,void * ctx)
{
    TRACE_IN();
	struct evbuffer* readbuffer = bufferevent_get_input(bev);
    owner->OnCmdCome((void*)bev,(const char *)evbuffer_pullup(readbuffer,-1),evbuffer_get_length(readbuffer),(BUS_ADDRESS_POINTER)ctx);
	evbuffer_drain(readbuffer,evbuffer_get_length(readbuffer));
	TRACE_OUT();
}

void ClientSocket::event_cb(struct bufferevent * bev,short events,void * ctx)
{
	TRACE_IN();
	
	evutil_socket_t fd = bufferevent_getfd(bev);
 	if(events & BEV_EVENT_CONNECTED)
 		{
 		    LOG_INFO("connect estiblised ,fd = %d",fd);
 			owner->OnConnect((void *)bev,0,(BUS_ADDRESS_POINTER)ctx);
 		}
	if(events & (BEV_EVENT_ERROR))
		{				 
            int errorcode =  evutil_socket_geterror(fd);
            LOG_ERROR("error(event = %d) happens on socket(fd = %d), error message: %s",(int)events,(int)fd,evutil_socket_error_to_string(errorcode));
			bufferevent_free(bev);
		}
	else if(events & (BEV_EVENT_EOF))
		{		
			LOG_INFO("EOF readed on socket(fd = %d)",(int)fd);
			bufferevent_free(bev);
		}
	else
		LOG_INFO("something unknow happens(event = %d ,fd = %d)",(int)events,(int)fd);	
	TRACE_OUT();
}

void ClientSocket::closeClient()
{
	bufferevent_free(_bev);
}
