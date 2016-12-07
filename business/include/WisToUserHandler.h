
#ifndef WIS_WISDEVICESTATUS_H
#define WIS_WISDEVICESTATUS_H

#include "wis.h"

#include "libLog.h"
#include "libSql.h"
#include "IUniteDataModule.h"
#include "transfer_structs.h"

class WisToUserHandler {
public:
    static void handleToUser( BUS_ADDRESS_POINTER busAddress,const char *uuid,int nDatalen,const char *pData );
    static void sendToUserResponse(BUS_ADDRESS_POINTER busAddress,int nCmd,int done );

    struct WisToUserData {
        int      flag;
        char     uuid[32];
        int      len;
        char     data[0];
    };
    static void handlePushMessage(BUS_ADDRESS_POINTER busAddress,const char *uuid,const WisToUserData *packet);
    static void handleSendToOne(BUS_ADDRESS_POINTER busAddress,const char *uuid,const WisToUserData *packet);
    static void handleSendToAll(BUS_ADDRESS_POINTER    busAddress,const char *uuid,const WisToUserData *packet);
};

#endif //WIS_WISDEVICESTATUS_H
