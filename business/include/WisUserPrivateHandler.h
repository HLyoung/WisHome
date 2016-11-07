

#ifndef WIS_WISUSERPRIVATEHANDLER_H
#define WIS_WISUSERPRIVATEHANDLER_H

#include "wis.h"
#include "libLog.h"
#include "libSql.h"
#include "transfer_structs.h"

class WisUserPrivateHandler {
public:
    static void handleUserPrivate(BUS_ADDRESS &busAddress,const char *uuid,int datalen,const char *pdata );
	
	static void sendUserPrivateResponse(BUS_ADDRESS &busAddress,int done);

private:

};


#endif //WIS_WISUSERPRIVATEHANDLER_H
