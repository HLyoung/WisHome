#ifndef _IUNITEDATAMODUEL_H_
#define _IUNITEDATAMODUEL_H_

#include "transfer_structs.h"
#include <list>
#include <string>

using namespace std;


//由应用实现的接口
class IAutoReportToInterface
{
public:
	//向应用返回信息
	virtual void ShowClientConnect(BUS_ADDRESS DeviceAddress) = 0;
	virtual void ShowClientDisConnect(BUS_ADDRESS DeviceAddress,std::string uuid,int loginType) = 0;
	virtual void ShowClientReceiveData(DEVICE_INFO & DeviceInfo,int nDataType,int nResultCode,int nDataLen,const char* pData) = 0;
	virtual void ShowDeviceScanResult(const char* pDeviceId, int nIp) = 0;
};

class ICUniteDataModule
{
public:
	//应用启动数据层模块
	virtual bool StartModule() = 0;
	//应用停止数据层模块
	virtual bool StopModule() = 0;
	//应用启动TCP服务器
	virtual bool StartTcpServer(int nPort) = 0;
	//应用注册接收数据模拟模块主动发送数据的对象接口
	virtual bool RegistInterfacRecObject(IAutoReportToInterface* pInterfaceObj) = 0;
	//应用手动连接服务器
	virtual bool ConnectServer(std::string strServerIp,int nServerPort) = 0;
	//应用驱动向网络对端发送数据（服务器或客户端)
	virtual bool SendData(std::string uuid, int nType, char* pSendData, int nDataSize, int nRole = TCP_CLIENT_MODE) = 0;
	virtual bool SendData(BUS_ADDRESS &busAddress, int nType, char* pSendData, int nDataSize, int nRole = TCP_CLIENT_MODE) = 0;
	
};

extern "C" ICUniteDataModule* GetUniteDataModuleInstance();
#endif