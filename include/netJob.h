
#ifndef NET_JOB_H
#define NET_JOB_H


#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include "macro.h"
#include "CJob.h"

class jobAccept:public CJob
{
private:
	struct bufferevent *_bev;
	BUS_ADDRESS_POINTER _pBus_address;
	CISocketOwner *_owner;
public:
	jobAccept(struct bufferevent *bev,BUS_ADDRESS_POINTER pBus_address,CISocketOwner *owner):\
	_bev(bev),_pBus_address(pBus_address),_owner(owner){
	}
	void Run(void){
	    _owner->OnAccept(_bev,_pBus_address);
	}
};

class jobRead:public CJob
{	
private:
	struct bufferevent *_bev;
	BUS_ADDRESS_POINTER _pBus_address;
	CISocketOwner *_owner;
	int _dataLen;
	char *_data;
public:
	jobRead(struct bufferevent *bev,BUS_ADDRESS_POINTER pBus_address,CISocketOwner *owner,int dataLen,char *data):_dataLen(dataLen),\
	_bev(bev),_pBus_address(pBus_address),_owner(owner){
		_data = new char[dataLen];
		if(NULL != _data)
			memcpy(_data,data,dataLen);
	}
	~jobRead(){
		if(NULL != _data){
			delete[] _data;
			}
	}
	
	void Run(void){
		if(NULL == _owner){
			LOG_ERROR("_owner is null");
			}
		if(NULL == _bev){
			LOG_ERROR("_bev is null");
			}
		else if(NULL == _data){
			LOG_ERROR("_data is null");
			}

		//if(NULL != _owner && NULL != _bev && NULL != _data)
			_owner->OnCmdCome(_bev,_data,_dataLen,_pBus_address);
	}
};

class jobClose:public CJob
{
private:
	BUS_ADDRESS_POINTER _pBus_address;
	CISocketOwner *_owner;
public:
	jobClose(BUS_ADDRESS_POINTER pBus_address,CISocketOwner* owner):_pBus_address(pBus_address),_owner(owner){}
	void Run(void){
		_owner->OnClose(_pBus_address);
	}
};


#endif
