

#ifndef WIS_WISDEVICECOMMANDMANAGER_H
#define WIS_WISDEVICECOMMANDMANAGER_H

#include "libSql.h"
#include "libLog.h"

class WisDeviceCommandDao {
public:
    static bool save( const std::string& user, const std::string& device, int cmd, int arglen, const std::string& arg );
private:

};


#endif //WIS_WISDEVICECOMMANDMANAGER_H
