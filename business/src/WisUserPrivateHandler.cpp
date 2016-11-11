
#include "WisDeviceCommandDao.h"
#include "WisUserPrivateHandler.h"
#include "IUniteDataModule.h"

struct WisUserPrivatePacket {
    char     uuid[32];
    int      cmdId;
    int      argLen;
    char     sender[32]; 
    char     arg[0];
};


void WisUserPrivateHandler::sendUserPrivateResponse(BUS_ADDRESS &busAddress,int done)
{
	int isDone = done;
    GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_USER_PRIVATE,(char *)&isDone,sizeof(int),TCP_SERVER_MODE);
}

void WisUserPrivateHandler::handleUserPrivate( BUS_ADDRESS &busAddress,const char *uuid,int datalen,const char *pdata )
{
    if( datalen < sizeof(WisUserPrivatePacket) ) {
        LOG_ERROR("invalid parameter!");
		sendUserPrivateResponse(busAddress,-1);
        return;
    }
    const WisUserPrivatePacket* userPrivate = reinterpret_cast<const WisUserPrivatePacket*>( pdata );  
	
    if(!GetUniteDataModuleInstance()->SendData( userPrivate->uuid,userPrivate->cmdId,(char *)userPrivate + 40,userPrivate->argLen,TCP_SERVER_MODE))
    {
    	LOG_ERROR("handle user private failed(dstUuid = %s,cmd = %d,datalen = %d)",userPrivate->uuid,userPrivate->cmdId,userPrivate->argLen);
     	WisDeviceCommandDao::save(uuid, userPrivate->uuid, userPrivate->cmdId, userPrivate->argLen, userPrivate->arg);
		sendUserPrivateResponse(busAddress,-1);
    }  
	else
		sendUserPrivateResponse(busAddress,0);
}
