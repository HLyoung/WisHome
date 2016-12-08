

#include "wisLogginHandler.h"

std::mutex WisLoginHandler::uMutex;
std::mutex WisLoginHandler::dMutex;
std::map<BUS_ADDRESS_POINTER,std::string> WisLoginHandler::mUser;
std::map<BUS_ADDRESS_POINTER,std::string> WisLoginHandler::mDevice;

void WisLoginHandler::sendLoginResponse(BUS_ADDRESS_POINTER busAddress,char *m_uuid,int done,int type )
{
  TRACE_IN();

  logResult result;
  memset(&result,0,sizeof(logResult));
  result.result = done;
  strncpy(result.uuid,m_uuid,UUID_LEN);

  int nCmd = 0;
  switch(type)
  	{
  		case TYPE_DEVICE:
			nCmd = WIS_CMD_LOGIN;
			break;
		case TYPE_USER:
			nCmd = WIS_CMD_USER_AUTO_LOGIN;
			break;
		default:
			return;
  	}
  GetUniteDataModuleInstance()->SendData(busAddress,nCmd,(char *)&result,sizeof(result),TCP_SERVER_MODE);

  TRACE_OUT();
}

void WisLoginHandler::handleUserLogin(BUS_ADDRESS_POINTER busAddress,int datalen,char *pdata )
{
	TRACE_IN();	
    WisUserLoginInfo* loginInfo = (WisUserLoginInfo*)(pdata);
	std::string uuid = getUuidFromBuffer(loginInfo->uuid);
	std::string password = getPasswordFromBuffer(loginInfo->password);
	
    if(WisUserDao::login(uuid, password)){
		mapAddUser(busAddress,uuid);
	    WisLogDao::saveUserLoginLog(uuid, strlen((char *)busAddress->host_address.ip),(const char*)busAddress->host_address.ip);
	    if ( datalen >= TOKEN_LEN + UUID_LEN + PASSWORD_LEN) {
	        std::string token(loginInfo->token);
			std::transform(token.begin(),token.end(),token.begin(),(int(*)(int))toupper);
			WisIOSTokenDao::save( loginInfo->uuid, token );
	    	}
	    sendLoginResponse( busAddress, loginInfo->uuid,0,TYPE_USER );
		return;
    	}
	sendLoginResponse( busAddress, loginInfo->uuid,-1,TYPE_USER );
	TRACE_OUT();
}
void WisLoginHandler::handleUserLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid)
{
   TRACE_IN();
   if(WisUserDao::logout(uuid, WisUserDao::defaultPassword)){
   		mapDeleteUser(busAddress);
  		WisLogDao::saveUserLogoutLog(uuid, 0, "");
   	    }
  TRACE_OUT();
}

void WisLoginHandler::handleKickoutUser(std::string uuid)
{
	TRACE_IN();
	BUS_ADDRESS_POINTER busAddress = getUserAddress(uuid);
	while(busAddress != NULL){
		mapDeleteUser(busAddress);
		GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_SERVICE_KICKOUT_USER,NULL,0,TCP_SERVER_MODE);	
		busAddress = getUserAddress(uuid);
		}
	TRACE_OUT();
}


void WisLoginHandler::handleDeviceLogin( BUS_ADDRESS_POINTER busAddress,int datalen,char *pdata )
{
	TRACE_IN();
    WisDeviceLoginInfo* loginInfo = (WisDeviceLoginInfo*)pdata;
	
    bool firstLogin = false;
    bool rc = WisDeviceDao::check(getUuidFromBuffer(loginInfo->uuid));
    if( !rc ) {
        rc = WisDeviceDao::regist(getUuidFromBuffer(loginInfo->uuid), std::string(loginInfo->name));
        if( !rc ) {
            LOG_ERROR("device(uuid = %s,name = %s )regist failed.",getUuidFromBuffer(loginInfo->uuid).c_str(),
            std::string(loginInfo->name,NAME_LEN).c_str());
            sendLoginResponse( busAddress,loginInfo->uuid, -1,TYPE_DEVICE );
            return;
        }
        firstLogin = true;
    }
    rc = WisDeviceDao::login(loginInfo->uuid,loginInfo->name );
    if( !rc ) {
        LOG_ERROR("devicee(uuid = %s,name = %s) login failed.",loginInfo->uuid,loginInfo->name);
        sendLoginResponse( busAddress, loginInfo->uuid,-1,TYPE_DEVICE );
        return;
    }
    if( firstLogin ) {
        WisLogDao::saveDeviceRegLog(loginInfo->uuid, strlen((char *)busAddress->host_address.ip), (const char *)busAddress->host_address.ip);
    } else {
        WisLogDao::saveDeviceLoginLog(loginInfo->uuid, strlen((char*)busAddress->host_address.ip),(const char*) busAddress->host_address.ip);
    } 
	mapAddDevice(busAddress,getUuidFromBuffer(loginInfo->uuid));
    sendLoginResponse( busAddress,loginInfo->uuid, 1 ,TYPE_DEVICE);	
    
    std:map<std::string,WisUserInfo> mapUserIfno;
	if(0 < WisBindDao::getBindedUsers(loginInfo->uuid,mapUserIfno))
	{
		for(std::map<std::string,WisUserInfo>::iterator ite = mapUserIfno.begin();\
			ite != mapUserIfno.end();ite++)
			{
			    BUS_ADDRESS_POINTER pBus_address = getUserAddress(getUuidFromBuffer(ite->second.uuid));
				if(pBus_address != NULL)
					GetUniteDataModuleInstance()->SendData(pBus_address,WIS_CMD_USER_DEV_LOGIN,loginInfo->uuid,\
					strlen(loginInfo->uuid),TCP_SERVER_MODE);
			}				
	}	
	TRACE_OUT();
}

void WisLoginHandler::handleDeviceLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid )
{
	TRACE_IN();
    if(WisDeviceDao::logout( uuid )){
		mapDeleteDevice(busAddress);
    	WisLogDao::saveDeviceLogoutLog(uuid, 0, "");

	    std:map<std::string,WisUserInfo> mapUserIfno;
		if(0 < WisBindDao::getBindedUsers(uuid,mapUserIfno)){
			for(std::map<std::string,WisUserInfo>::iterator ite = mapUserIfno.begin();ite != mapUserIfno.end();ite++){
					BUS_ADDRESS_POINTER pBus_address = getUserAddress(ite->second.uuid);
					if(pBus_address != NULL)
						GetUniteDataModuleInstance()->SendData(pBus_address,WIS_CMD_USER_DEV_LOGOUT,(char *)uuid.c_str(),strlen(uuid.c_str()),TCP_SERVER_MODE);
					}			
				}	
    	}
	TRACE_OUT();
}

bool WisLoginHandler::isUserLogin(const std::string&uuid)
{
	std::lock_guard<std::mutex> lg(uMutex);
	std::map<BUS_ADDRESS_POINTER ,std::string>::iterator ite = mUser.begin();
	for(;ite != mUser.end(); ite++)
		if(ite->second == uuid)
			return true;

	return false;
}

bool WisLoginHandler::isDeviceLogin(const  string & uuid)
{
	std::lock_guard<std::mutex> lg(dMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mDevice.begin();
	for(; ite != mDevice.end();ite++)
		if(ite->second == uuid)
			return true;
	return false;
}

bool WisLoginHandler::isLogin(const std::string &uuid)
{
	if(isUserLogin(uuid) || isDeviceLogin(uuid))
		return true;
	return false;
}

BUS_ADDRESS_POINTER  WisLoginHandler::getUserAddress(const  string & uuid)
{
	std::lock_guard<std::mutex> lg(uMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mUser.begin();
	for(;ite != mUser.end();ite++)
		if(ite->second == uuid)
			return ite->first;
	
	return NULL;
}

BUS_ADDRESS_POINTER WisLoginHandler::getDeviceAddress(const  string & uuid)
{
	std::lock_guard<std::mutex> lg(dMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mDevice.begin();
	for(; ite != mDevice.end(); ite++)
		if(ite->second == uuid)
			return ite->first;

	return NULL;
}

void WisLoginHandler::mapAddUser(BUS_ADDRESS_POINTER bus_address,const std::string& uuid)
{
	std::lock_guard<std::mutex> lg(uMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mUser.begin();
	for(;ite != mUser.end();){
		if(ite->second == uuid){
			mUser.erase(ite);
			GetUniteDataModuleInstance()->SendData(ite->first,WIS_CMD_SERVICE_KICKOUT_USER,NULL,0,TCP_SERVER_MODE);	
			}
		ite++;
		}		
	mUser[bus_address] = uuid;
}

void WisLoginHandler::mapAddDevice(BUS_ADDRESS_POINTER bus_address, const std::string & uuid)
{
	std::lock_guard<std::mutex> lg(dMutex);
	mDevice[bus_address] = uuid;
}

void WisLoginHandler::mapDeleteUser(BUS_ADDRESS_POINTER bus_address)
{
	std::lock_guard<std::mutex> lg(uMutex);
	if(mUser.find(bus_address) != mUser.end())
		mUser.erase(bus_address);
}

void WisLoginHandler::mapDeleteSameUser(const std::string &uuid)
{
	std::lock_guard<std::mutex> lg(uMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mUser.begin();
	for(;ite != mUser.end();){
		if(ite->second == uuid)
			mUser.erase(ite);
		ite++;
		}	
}


void WisLoginHandler::mapDeleteDevice(BUS_ADDRESS_POINTER bus_address)
{
	std::lock_guard<std::mutex> lg(dMutex);
	if(mDevice.find(bus_address) != mDevice.end())
		mDevice.erase(bus_address);
}