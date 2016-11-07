

#ifndef _COMMON_H_
#define _COMMON_H_

#include "ISocketOwner.h"
#include <stdio.h>
#include <libLog.h>
#include "XML/tinyxml.h"

#define safe_free(ptr) \
	do{\
		if(NULL != ptr)\
			{\
				free(ptr); \
				ptr = NULL;\
			}\
		}while(0);
		

typedef enum _CLIENT_SOCKET_STATUS_
{
	ERROR = 0,
	BF_CONNECT = 1,
	CONNECTED = 2,
	CLOSED = 3,
}SOCKET_CONNECT_STATUS;

class sLink
{
public:
	int getFd() const
	{
		return fd;
	}
	void setFd(int nfd)
	{
		fd = nfd;
	}
	CADDRINFO  getRemoAddr() const
	{
		return m_RemoteAddr;
	}
	void setRemoAddr(CADDRINFO &addr)
	{
		m_RemoteAddr = addr;
	}
	sLink& operator = (const sLink &target);
	bool operator==(const sLink &target);
	
private:
	int fd;
	CADDRINFO m_RemoteAddr;
};


#endif
