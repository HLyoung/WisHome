
#include "WisDeviceCommandDao.h"
#include "WisUserPrivateHandler.h"
#include "IUniteDataModule.h"
#include "wisLogginHandler.h"

struct WisUserPrivatePacket {
    char     uuid[32];
    int      cmdId;
    int      argLen;  //len include sender.len
    char     sender[32]; 
    char     arg[0];
};


void WisUserPrivateHandler::sendUserPrivateResponse(BUS_ADDRESS_POINTER busAddress,int done)
{
	int isDone = done;
    GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_USER_PRIVATE,(char *)&isDone,sizeof(int),TCP_SERVER_MODE);
}

void WisUserPrivateHandler::handleUserPrivate( BUS_ADDRESS_POINTER  busAddress,const char *uuid,int datalen,const char *pdata )
{
	TRACE_IN();
    const WisUserPrivatePacket* userPrivate = reinterpret_cast<const WisUserPrivatePacket*>( pdata ); 
	string devID = getUuidFromBuffer(userPrivate->uuid);
	string userID = getUuidFromBuffer(userPrivate->sender);
	BUS_ADDRESS_POINTER pbusAddress = WisLoginHandler::getDeviceAddress(devID);
	if(pbusAddress != NULL){
		if(!GetUniteDataModuleInstance()->SendData(pbusAddress,userPrivate->cmdId,(char *)userPrivate + 40,userPrivate->argLen,TCP_SERVER_MODE)){
     		WisDeviceCommandDao::save(uuid, userID, userPrivate->cmdId, userPrivate->argLen, userPrivate->arg);
			sendUserPrivateResponse(busAddress,-1);
			LOG_INFO("USER PRIVATE FAILED: devID=%s,userID=%s,cmd=%d",devID.c_str(),userID.c_str(),userPrivate->cmdId);
			}
		else{
			sendUserPrivateResponse(busAddress,0);
			LOG_INFO("USER PRIVATE SUCCESS: devID=%s,userID=%s,cmd=%d",devID.c_str(),userID.c_str(),userPrivate->cmdId);
			}
		}
    LOG_INFO("USER PRIVATE FAILED: devID=%s,userID=%s,cmd=%d",devID.c_str(),userID.c_str(),userPrivate->cmdId);
	TRACE_OUT();
}
