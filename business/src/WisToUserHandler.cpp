
#include "WisIOSPushDao.h"
#include "WisBadgeCache.h"
#include "WisToUserHandler.h"
#include "WisIOSTokenDao.h"
#include "WisBindDao.h"
#include "JPush.h"

#include "libLog.h"
#include "libSql.h"

struct ServerSendToUser
{
	char fromUuid[UUID_LEN];
	char data[0];
};

void WisToUserHandler::handlePushMessage(BUS_ADDRESS &busAddress,const char *uuid,const WisToUserData *packet)
{

	TRACE_IN();
  
	std::map<std::string,WisUserInfo> mapUser;
	if( WisBindDao::getBindedUsers(packet->uuid,mapUser) <= 0)
	{
		LOG_INFO("the device(uuid = %s) binded no user",uuid);
		sendToUserResponse(busAddress, WIS_CMD_TO_USER, 0);
		return;
	}
	
	std::vector<std::string> deviceUUIDs;
	std::map<std::string ,WisUserInfo>::iterator mapuserIter = mapUser.begin();	
	for(;mapuserIter != mapUser.end();mapuserIter++)
	{
		deviceUUIDs.push_back(mapuserIter->second.uuid);
	}
	
	std::set<std::string> tokens;
	WisIOSTokenDao::getTokens(deviceUUIDs,tokens);
	if(!tokens.empty()){	
		int errCode = (int)JPush::push_SpecifiedIDs(std::string(packet->data,packet->len),"device message",1,tokens);
		if(CURLE_OK != errCode)
			LOG_ERROR("push message to users failed,erroCode:%d",errCode);
	}
    sendToUserResponse(busAddress, WIS_CMD_TO_USER, 0);
    WisIOSPushDao::save(uuid, packet->flag, packet->len,std::string(packet->data,packet->len));

	TRACE_OUT();
}

void WisToUserHandler::handleSendToOne(BUS_ADDRESS &busAddress,const char *uuid, const WisToUserData* packet)
{
	TRACE_IN();

    char deviceUUID[UUID_LEN + 1] = {0};
	memcpy(deviceUUID, packet->uuid, UUID_LEN);
    LOG_INFO("handle(srcuuid = %s) send to one(dstuuid = %s,len = %d)",uuid,deviceUUID,packet->len);

	char *toUser =  new char[UUID_LEN + packet->len];
	memcpy((char *)toUser,uuid,UUID_LEN);
	memcpy((char *)toUser + UUID_LEN,packet->data,packet->len);
    
	if(GetUniteDataModuleInstance()->SendData(std::string(deviceUUID),WIS_CMD_TO_USER,toUser,\
	UUID_LEN + packet->len,TCP_SERVER_MODE))
	{
		LOG_INFO("send message to one(uuid = %s )success ",deviceUUID);
		sendToUserResponse(busAddress, WIS_CMD_TO_USER, 0);
	}
	else
	{
		LOG_INFO("send message to one(uuid = %s )failed ",deviceUUID);
		sendToUserResponse(busAddress, WIS_CMD_TO_USER, -1);
	}

	SafeDeleteArray(toUser);
	TRACE_OUT();
}

void WisToUserHandler::handleSendToAll(BUS_ADDRESS &busAddress,const char *uuid, const WisToUserData* packet)
{

	TRACE_IN();
	char * toUser = new char[UUID_LEN + packet->len];
	
	memcpy(toUser,uuid,UUID_LEN);
	memcpy(toUser + UUID_LEN,packet->data,packet->len);
	
    std::map<std::string, WisUserInfo>  DeviceBuddiesMap;
	WisBindDao::getBindedUsers(uuid,DeviceBuddiesMap);
	
	std::map<std::string ,WisUserInfo>::iterator iter = DeviceBuddiesMap.begin();
	for(;iter != DeviceBuddiesMap.end();iter ++)
	{
		GetUniteDataModuleInstance()->SendData(iter->second.uuid,WIS_CMD_TO_USER,toUser,\
											   UUID_LEN + packet->len,TCP_SERVER_MODE);
	}
	
    sendToUserResponse(busAddress, WIS_CMD_TO_USER, 0);
    SafeDeleteArray(toUser);
	TRACE_OUT();
}

void WisToUserHandler::handleToUser(BUS_ADDRESS &busAddress,const char *uuid,int nDataLen,const char *pData  )
{
	TRACE_IN();
	
	const WisToUserData *userData = (const WisToUserData*)(pData);

    char dstuuid[UUID_LEN + 1] = {0};
    memcpy(dstuuid,userData->uuid,UUID_LEN);
	LOG_INFO("handle message(flag = %d,len = %d ) device(ip = %s,uuid = %s) send to user(uuid = %s)",userData->flag,userData->len,\
			busAddress.host_address.ip,uuid,dstuuid);

    if( nDataLen < sizeof(WisToUserData) ) {
        sendToUserResponse(busAddress, WIS_CMD_TO_USER, -1);
        return;
    }
    

    if( userData->flag == CMD_FLAG_ISO_APNS_ALL_OFFLINE ) {
         handlePushMessage(busAddress,uuid,userData);
        return;
    }
    if( userData->flag == CMD_FLAG_USER_ONE ) {
        handleSendToOne(busAddress,uuid,userData );
        return;
    }
    if( userData->flag == CMD_FLAG_USER_ALL ) {
        handleSendToAll(busAddress,uuid,userData );
        return;
    }
	
    sendToUserResponse(busAddress, WIS_CMD_TO_USER, -1);
	TRACE_OUT();
}

void WisToUserHandler::sendToUserResponse(BUS_ADDRESS &busAddress,int nCmd,int done)
{
	TRACE_IN();
	int isDone = done;
    GetUniteDataModuleInstance()->SendData(busAddress,nCmd,(char *)&isDone,sizeof(int),TCP_SERVER_MODE);
	TRACE_OUT();
}
