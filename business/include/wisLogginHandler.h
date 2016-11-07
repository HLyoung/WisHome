
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
    static void handleUserLogin( BUS_ADDRESS &busAddress,int datalen,char *pdata);
    static void handleUserLogout(BUS_ADDRESS &busAddress,std::string uuid);

    static void handleDeviceLogin( BUS_ADDRESS &busAddress,int datalen,char *pdata);
    static void handleDeviceLogout(BUS_ADDRESS &busAddress,std::string uuid);



    static void sendLoginResponse(BUS_ADDRESS &busAddress, char *uuid,int done,int type );

private:

};

#endif //WIS_WISLOGINMANAGER_H
