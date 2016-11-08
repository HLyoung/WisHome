


#ifndef _UNITEDATA_MODULE_H_
#define _UNITEDATA_MODULE_H_

#include "macro.h"
#include "libLog.h"
#include "deviceManager.h"
#include "libSql.h"
#include "wis.h"
#include "transfer_structs.h"
#include "IUniteDataModule.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "TimerThread.h"

#include <unistd.h>
#include <iostream>

class CUniteDataModule : public ICUniteDataModule
{
public:
	static CUniteDataModule *GetInstance();
	virtual bool StartModule();
	virtual bool StartTimerModule();
	virtual bool StartTcpServer(int nPort);
	virtual bool StopModule();
	virtual bool RegistInterfacRecObject(IAutoReportToInterface* pInterfaceObj);
	virtual bool ConnectServer(string strServerIp,int nServerPort);
	virtual bool SendData(string uuid,int nDataType,char* pSendData,int nDataSize,int nRole = TCP_CLIENT_MODE);
	virtual bool SendData(BUS_ADDRESS & busAddress,int nDataType,char *pSendData,int nDataSize,int nRole = TCP_CLIENT_MODE);

	//向界面回调数据和状态
	bool ShowNetDataToInterface(DEVICE_INFO& GatewayAddress, int nDataType, int nResultCode, int nDataLen,const char* pData);
	void ShowClientConnect(BUS_ADDRESS GatewayAddress);
	void ShowClientDisConnect(BUS_ADDRESS GatewayAddress,std::string uuid,int loginType);

	IAutoReportToInterface* GetInterfaceObj();
	
	void GetConfigXml(CXmlDocument &xmldocument);
  
private:
	CUniteDataModule(void);
	virtual ~CUniteDataModule(void);
	// 数据库模块初始化
	bool DatabaseModuleInit();
	bool LogModuleInit();
	bool LoadConfigFile();
private:
	//单件对象
	static CUniteDataModule* m_Instance;
	//应用注册过来的对象句柄
	IAutoReportToInterface* m_pInterfaceCallback;
	bool m_bStarted;
	CXmlDocument m_ConfigXmlFile;
};
#endif