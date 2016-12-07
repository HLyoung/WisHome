
#ifndef WIS_WISLOGINMANAGER_H
#define WIS_WISLOGINMANAGER_H

#include "wisUserDao.h"
#include "WisDeviceDao.h"
#include "wisLogDao.h"
#include "wis.h"
#include "WisIOSTokenDao.h"
#include "IUniteDataModule.h"
#include "transfer_structs.h"
#include  <string>

#include <algorithm> 


class WisLoginHandler {
public:
    static void handleUserLogin( BUS_ADDRESS_POINTER busAddress,int datalen,char *pdata);
    static void handleUserLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid);
	static void handleKickoutUser(std::string uuid);

    static void handleDeviceLogin( BUS_ADDRESS_POINTER  busAddress,int datalen,char *pdata);
    static void handleDeviceLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid);
    static void sendLoginResponse(BUS_ADDRESS_POINTER busAddress, char *uuid,int done,int type );

	static bool isUserLogin(const std::string& uuid);
	static bool isDeviceLogin(const std::string &uuid);
	static bool isLogin(const std::string &uuid);

	static BUS_ADDRESS_POINTER getUserAddress(const std::string &uuid);   //一个用户可能在多个终端登录。
	static BUS_ADDRESS_POINTER getDeviceAddress(const std::string &uuid);

	static void mapAddUser(BUS_ADDRESS_POINTER bus_address,const std::string &uuid);
	static void mapAddDevice(BUS_ADDRESS_POINTER bus_address,const std::string &uuid);
	static void mapDeleteUser(BUS_ADDRESS_POINTER bus_address);
	static void mapDeleteSameUser(const std::string &uuid);
	static void mapDeleteDevice(BUS_ADDRESS_POINTER bus_address);
    
private:
	static std::map<BUS_ADDRESS_POINTER,std::string> mUser;
	static std::map<BUS_ADDRESS_POINTER,std::string> mDevice;
	static std::mutex uMutex;
	static std::mutex dMutex;

};

#endif //WIS_WISLOGINMANAGER_H
