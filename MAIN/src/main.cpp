
#include "IUniteDataModule.h"
#include "wisLogginHandler.h"
#include "WisHeartBeatHandler.h"
#include "WisToUserHandler.h"
#include "WisUserPrivateHandler.h"
#include "wis.h"
#include "libLog.h"
#include "JPush.h"
#include <iostream>
#include <unistd.h>
using namespace std;



class Reporter : public IAutoReportToInterface
{
	void ShowClientConnect(BUS_ADDRESS_POINTER DeviceAddress){
		LOG_INFO("LINK CONNECTED: ip=%s,port=%d",DeviceAddress->host_address.ip,DeviceAddress->host_address.port);
	}
	void ShowClientDisConnect(BUS_ADDRESS_POINTER busAddress,std::string uuid,int loginType){
 
		TRACE_IN();
		
		switch(loginType)
		{
			case TYPE_DEVICE:
				WisLoginHandler::handleDeviceLogout(busAddress,uuid);
				break;
			case TYPE_USER:
				WisLoginHandler::handleUserLogout(busAddress,uuid);
				break;
			default:
				break;
		}
		TRACE_OUT();
	}
	void ShowClientReceiveData(DEVICE_INFO& DeviceInfo,int nDataType,int nResultCode,int nDataLen,const char* pData){
		TRACE_IN();		
		switch(nDataType)
		{
			case WIS_CMD_LOGIN:
			    WisLoginHandler::handleDeviceLogin(DeviceInfo.bus_address,nDataLen,(char *)pData);
				break;
			case WIS_CMD_USER_AUTO_LOGIN:
				WisLoginHandler::handleUserLogin(DeviceInfo.bus_address,nDataLen,(char *)pData);
				break;
			case WIS_CMD_USER_REGIST:
				WisUserDao::handleUserRegist(DeviceInfo.bus_address,pData);
				break;
			case WIS_CMD_USER_QUIT:
				WisUserDao::handleUserQuit(DeviceInfo.bus_address,pData);
				break;
			case WIS_CMD_USER_MODIFY_PASSWORD:
				WisUserDao::handleUserModifyPassword(DeviceInfo.uuid,DeviceInfo.bus_address,pData);
				break;
			case WIS_CMD_USER_RESET_PASSWORD:
				WisUserDao::handleUserResetPassword(DeviceInfo.bus_address, pData);
				break;
			case WIS_CMD_USER_BIND:
				WisBindDao::addBind(DeviceInfo.bus_address,DeviceInfo.uuid,getUuidFromBuffer(pData));
				break;
			case WIS_CMD_USER_UNBIND:
				WisBindDao::delBind(DeviceInfo.bus_address,DeviceInfo.uuid,getUuidFromBuffer(pData));
				break;
			case WIS_CMD_HEART_BEAT:
				WisHeartBeatHandler::handleDeviceHeartBeat(DeviceInfo.bus_address);
				break;
			case WIS_CMD_TO_USER:
				WisToUserHandler::handleToUser(DeviceInfo.bus_address,DeviceInfo.uuid.c_str(),nDataLen,pData);
				break;
			case WIS_CMD_USER_GET_DEVICES:
				WisUserDao::userGetDevice(DeviceInfo.bus_address,DeviceInfo.uuid.c_str());
				break;
			case WIS_CMD_USER_PRIVATE:
				WisUserPrivateHandler::handleUserPrivate(DeviceInfo.bus_address,DeviceInfo.uuid.c_str(),nDataLen,pData);
				break;
			
			default:
				break;
		}
		TRACE_OUT();
	}
	void ShowDeviceScanResult(const char* pDeviceId, int nIp){
		std::cout<<"scan result"<<endl;
	}
	
};

int main()
{
	ICUniteDataModule * iModuel = GetUniteDataModuleInstance();
	
	if(iModuel == NULL)
	{
		std::cout<<"NULL"<<std::endl;
		return  0;
	}
	if(!iModuel->StartModule())
	{
		std::cout<<"some module start failed"<<std::endl;
		return 0;
	}

	Reporter report;
	iModuel->RegistInterfacRecObject(&report);
	
	iModuel->StartTcpServer(7788);	
	while(1)
	{
		sleep(5);
	}
	iModuel->StopModule();
	
	return 0;
}
