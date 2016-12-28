
#ifndef _COMMONTCPSERVERMANAGER_H_
#define _COMMONTCPSERVERMANAGER_H_

#include "Tcpmodule_Config.h"
#include "ISocketOwner.h"
#include "transfer_structs.h"
#include "Common.h"
#include "ISocket.h"
#include "macro.h"
#include "wis.h"
#include "CommonTCPManager.h"
#include "netJob.h"
#include <list>
#include <map>


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <linux/tcp.h>



#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

class CommonTCPManager;
class ServrSocket : public ISocket
{
public:
    ServrSocket(const CADDRINFO &sveAddr,CISocketOwner* servr_owner,CommonTCPManager *manager);
	~ServrSocket();
	static void* startServrThread(void *p);
	int startServr();
    CADDRINFO getLocalAddr() const
	{
		return m_LocalAddr;
	}
	void setLocalAddr(CADDRINFO addr)
	{
		m_LocalAddr = addr;
	}
	CISocketOwner * getOwner() 
	{
		return owner;
	}

	CommonTCPManager *getManager(){return manager;}
	void setOwner(CISocketOwner *serv_owner)
	{
		owner = serv_owner;
	}
    void bufMapAddBuf(struct bufferevent *bev,struct event *timeout);
	
	bool bufMapDeleteBuf(void *bev);

	int  bufSend(struct bufferevent *bev,const char *data,int dataLen);
	static void accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *address, int socklen, void *ctx);

	static void read_cb(struct bufferevent *bev,void *ctx);
	static void write_cb(struct bufferevent *bev,void *ctx);

	static void accept_error_cb(struct evconnlistener *listener,void *ctx);

	static void event_cb(struct bufferevent *bev,short events,void *ctx);

	static void timer_cb(int fd,short event,void *ctx);

	

	
	
private:
	std::map<struct bufferevent*,struct event *> bufMap;
	std::map<struct bufferevent*,int> mCounter;    //count lose heart beat times.
	std::mutex bufMapMutex;
	
	CADDRINFO  m_LocalAddr; 
	static CISocketOwner *owner;
	CommonTCPManager *manager;
};


#endif
