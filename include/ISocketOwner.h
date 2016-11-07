
#ifndef _ISOCKETOWNER_H_
#define _ISOCKETOWNER_H_

#include "Tcpmodule_Config.h"
#include "transfer_structs.h"
#ifdef WIN32
#include <wtype.h>
#endif

#define IP_LEN 50

class DLL_DEFINE CADDRINFO
{
private:
	char m_pszIP[IP_LEN];
	int m_nPort;
public:
	char *GetIP() const;
	int GetPort() const;
	int SetIPAddr(const char *ipTemp);
	int SetPortAddr(int port);
	
	CADDRINFO();
	CADDRINFO(const CADDRINFO &target);
	CADDRINFO(const char* pszIP,int nPort);
	
	CADDRINFO& operator = (const CADDRINFO &target);
	int operator == (const CADDRINFO &target) const;   //返回1表示相等，返回0表示不相等
	
};	

/**
 * @class CISocketOwner
 *
 *@brief TCP网络消息通知接口类
 *
 */
 class CISocketOwner
 {
public:
	virtual ~CISocketOwner(){};
	/**
	* @brief 网络数据到达(服务器/客户端)
	*
	* @param[in] dwHandle socket句柄
	* @param[in] pData 数据缓冲区
	* @param[in] nLen 数据长度
	* @param[in] SocketInfo 地址信息
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int OnCmdCome(void * dwHandle,const char *pData,int nLen,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 连接关闭(服务器/客户端)
	*
	* @param[in] dwHandle socket句柄
	* @param[in] SocketInfo 地址信息
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int OnClose(void* dwHandle,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 连接成功(客户端)
	* @param[in] dwHandle socket句柄
	* @param[in] SocketInfo 地址信息
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int OnConnect(void * dwHandle,int OwnerType,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 网络错误(服务器/客户端)
	*
	* @param[in] dwHandle socket句柄
	* @param[in] nErrorNo 错误号
	* @param[in] SocketInfo 地址信息
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int OnError(void * dwHandle,int & nErrorNo,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 连接超时(服务器/客户端)
	*
	* @param[in] dwHandle socket句柄
	* @param[in] SocketInfo 地址信息
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int OnTimeOut(void * dwHandle,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 连接被接受(服务器)
	*
	* @param[in] dwHandle socket句柄
	* @param[in] SocketInfo 地址信息
	*
	* @return ！NULL 成功
	* @return NULL 失败
	*/
	virtual CISocketOwner * OnAccept(void * dwHandle,BUS_ADDRESS_POINTER pBus_address) = 0;
	
	/**
	* @brief 通知上层Owner可以释放(服务器/客户端)
	* 
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int Release() = 0;
	
};

#endif