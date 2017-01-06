//
// Created by derry on 15-12-1.
//

#ifndef WIS_WISDEVICEMANAGER_H
#define WIS_WISDEVICEMANAGER_H

#include <string>
#include <stdio.h>
#include <string.h>
#include "WisBindDao.h"

#include "libSql.h"
#include "libLog.h"
#include "CNVDataAccess.h"

class WisDeviceDao {
public:
    static bool check( const std::string& uuid );
    static bool regist( const std::string& uuid, const std::string& name);
    static bool login( const std::string& uuid,const std::string &name  );
    static bool logout( const std::string& uuid );
    static bool logoutAll( );
private:
};


#endif //WIS_WISDEVICEMANAGER_H
