
#ifndef _NETEXT_H_
#define _NETEXT_H_

#include "ISocketOwner.h"

/** 
	* @class CICommonTCPManager
	* 包括TCP服务器与客户端
	* 支持window与linux
*/

class DLL_DEFINE CICommonTCPManager
{
public:
	/** 
	* @brief BeginServer
	*
	* 开始服务器监听(服务器)
	* 
	* @param[in] svrAddr 服务器地址
    * @param[in] pSocketOwner 服务器Owner指针
    * 接受成功时，pSocketOwner指向的对象的OnAccept方法被调用
    * OnAccept方法返回新的CISocketOwner对象指针，用于与客服端通信
    *
	* @return 0 成功
	* @return <0 失败
	*/	
	virtual int BeginServer(const CADDRINFO &svrAddr, CISocketOwner *pSocketOwner) = 0;

	/**
	* @brief ConnectServer
	* 
	* 连接TCP服务器(客服端)
	*
	* @param[in] svrAddr 服务器地址
	* @param[in] pSocketOwner 客户端Owner指针，用于网络消息接收
	* @param[in] bLongConnect 0-短连接(不重连)，1- 长连接(自动重连)
	* @param[in] nTimeOut 超时时间(秒), nTimeOut未收到数据关闭连接
	* @return >0 成功，返回连接句柄
	* @return <0 失败
	 */ 
	virtual int ConnectServer(const CADDRINFO &svrAddr,CISocketOwner *pSocketOwner,int bLongConnect = 0,int nTimeOut = 120)=0;
	
	/**
	*
	* @brief CloseConnection
	*
	* 关闭TCP连接(客户端)
	*
	* @param[in] dwHandle 连接句柄
	*
	* @retuan 0 成功
	* @return <0 失败
	*/
	virtual int CloseConnection(void* dwHandle) = 0;
	
	/**
	* @brief Send
	*
	* 发送数据(服务器/客户端)
	*
	* @param[in] dwHandle 连接句柄
	* @param[in] pData 发送数据缓冲区
	* @param[in] nProLen 发送数据长度
	*
	* @return 0 成功
	* @return <0 失败
	*/
	virtual int Send(void *handle,const char *pData,int nProLen) = 0;
};

DLL_DEFINE CICommonTCPManager *GetCommonTCPManager();

#endif



 