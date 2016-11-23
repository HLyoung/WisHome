
#ifndef _TCPSERVICE_H_
#define _TCPSERVICE_H_

#include "macro.h"
#include "ICommonTCPManager.h"

#include "HostAddressMap.h"
#include "transfer_structs.h"
#include "ISocketOwner.h"
#include "libLog.h"

class CTCPService : public CISocketOwner
{
	enum {default_time_out = 60*300};
	
public:
	CTCPService();
	virtual ~CTCPService();
	
	virtual int OnCmdCome( void *handle,const char *pData,int nLen,BUS_ADDRESS_POINTER pBus_address);
	virtual int OnClose(void* handle,BUS_ADDRESS_POINTER pBus_address);
	virtual int OnConnect(void *handle,int ownertype,BUS_ADDRESS_POINTER pBus_address);
	virtual int OnError(void *Handle,int &nErrorNo,BUS_ADDRESS_POINTER pBus_address);
	virtual int OnTimeOut(void* dwHandle,BUS_ADDRESS_POINTER pBus_address);
	virtual CISocketOwner *OnAccept(void* handle,BUS_ADDRESS_POINTER pBus_address);
	virtual int Release();

	void SetIp(std::string ip){ m_strIp = ip;}
	std::string GetIp(void ){ return m_strIp;}

	void SetPort(int port){m_port = port;}
	int GetPort(void ){return m_port;}
	
	bool InitServer(void* owner,int nPort,BUS_SERVICE_MESSAGE_CALLBACK callback);
	bool InitClient(void* owner,string strIp,int nPort,int nTCPServiceType,BUS_SERVICE_MESSAGE_CALLBACK callback);
	
	int GetServiceModel();
	void SetServiceModel(int nModel);

	int GetTimeout();
	void* GetOwnerHandle();
	
	bool GetTcpServiceValid();
	void SetTcpServiceValid(bool bValid);	
	
private:
	int m_port;
	string m_strIp;
	int m_timeout;
	void* owner_;
	int m_TcpServiceMode;
	bool m_bValid;
	std::mutex m_ValidMutex;

public:
	BUS_SERVICE_MESSAGE_CALLBACK message_callback_;
};

#endif