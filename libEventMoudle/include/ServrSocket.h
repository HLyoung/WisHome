
#ifndef _COMMONTCPSERVERMANAGER_H_
#define _COMMONTCPSERVERMANAGER_H_

#include "Tcpmodule_Config.h"
#include "ISocketOwner.h"
#include "transfer_structs.h"
#include "Common.h"
#include "ISocket.h"
#include "macro.h"
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


class ServrSocket : public ISocket
{
public:
    ServrSocket(const CADDRINFO &sveAddr,CISocketOwner* servr_owner);
	~ServrSocket();
	static void* startServrThread(void *p);
	static void* handle_signel_connct(void *p);

	int startServr();
    CADDRINFO getLocalAddr() const
	{
		return m_LocalAddr;
	}
	void setLocalAddr(CADDRINFO addr)
	{
		m_LocalAddr = addr;
	}
	const CISocketOwner * getOwner() 
	{
		return owner;
	}
	void setOwner(CISocketOwner *serv_owner)
	{
		owner = serv_owner;
	}
    void bufMapAddBuf(struct bufferevent *bev,BUS_ADDRESS_POINTER pBusAddress);
	bool bufMapDeleteBuf(struct bufferevent *bev);
	
	static void accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *address, \
		                int socklen, void *ctx);

	static void read_cb(struct bufferevent *bev,void *ctx);
	static void write_cb(struct bufferevent *bev,void *ctx);

	static void accept_error_cb(struct evconnlistener *listener,void *ctx);

	static void event_cb(struct bufferevent *bev,short events,void *ctx);

	void closeServer(struct bufferevent *bev);

	std::map<struct bufferevent*,BUS_ADDRESS_POINTER> bufMap;
	std::mutex bufMapMutex;
private:
	CADDRINFO m_LocalAddr;
	static CISocketOwner *owner;
	
	
};


#endif
