

#ifndef _COMMONTCPCLIENTMANAGER_H_
#define _COMMONTCPCLIENTMANAGER_H_ 


#include "Tcpmodule_Config.h"
#include "ISocketOwner.h"
#include "Common.h"
#include "ISocket.h"
#include "macro.h"
#include "transfer_structs.h"


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


class  CommonTCPManager;
class ClientSocket : public ISocket
{
public:
	ClientSocket(const CADDRINFO &clientAddr,const CISocketOwner *client_owner,CommonTCPManager *manager);
	static void *startClientThread(void *p);
	int startClient();
	ClientSocket&  socket_Init(const CADDRINFO &sveAddr);

	CADDRINFO getRemoAddr(void) const
	{
		return m_RemoteAddr;
	}

	const CISocketOwner * getOwner() 
	{
		return owner;
	}
	void setOwner(CISocketOwner *serv_owner)
	{
		owner = serv_owner;
	}

	static void read_cb(struct bufferevent *bev,void *ptr);

	static void event_cb(struct bufferevent *bev,short events,void *ptr);
	
private:
	CADDRINFO m_RemoteAddr;
	static CISocketOwner *owner;
	CommonTCPManager *manager_;
};




#endif
