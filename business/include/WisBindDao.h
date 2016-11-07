//
// Created by derry on 15-12-2.
//

#ifndef WIS_WISBINDMANAGER_H
#define WIS_WISBINDMANAGER_H

#include <string>
#include <string.h>

#include <vector>

#include "wis.h"
#include "libSql.h"
#include "libLog.h"
#include "CNVDataAccess.h"
#include "IUniteDataModule.h"

#include "wisLogDao.h"

struct WisDeviceInfo {
    char    uuid[32];
    char    name[32];
    char    time[20];
    int     status;
};

struct WisUserInfo {
    char  uuid[32];
    char  time[20];
    int   status;
};

class WisBindDao {
public:
    static bool addBind( const std::string& userUUID, const std::string& deviceUUID );
    static bool delBind( const std::string& userUUID, const std::string& deviceUUID );
    static int  getBindedDevices( const std::string& userUUID, std::map<std::string,WisDeviceInfo>& buddies );
    static int  getBindedUsers( const std::string& devUUID, std::map<std::string,WisUserInfo>& buddies );
	static void sendBindResponse(const std::string &userUUID,int nCmd, int done );
};

#endif //WIS_WISBINDMANAGER_H
