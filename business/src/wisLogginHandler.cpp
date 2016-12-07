

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
  strncpy(result.uuid,m_uuid,32);

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
	handleKickoutUser(std::string(loginInfo->uuid));      //若一个用户在不同终端登录，先将用户踢出。
	
    bool deviceExists = WisUserDao::login(std::string(loginInfo->uuid,UUID_LEN), std::string(loginInfo->password,PASSWORD_LEN));
    if( !deviceExists ) {
        LOG_INFO("user(uuid = %s) login failed",std::string(loginInfo->uuid,UUID_LEN).c_str());
        sendLoginResponse( busAddress, loginInfo->uuid,1,TYPE_USER);
        return;
    } 
	mapAddUser(busAddress,std::string(loginInfo->uuid));
    WisLogDao::saveUserLoginLog(loginInfo->uuid, strlen((char *)busAddress->host_address.ip),(const char*)busAddress->host_address.ip);
    if ( datalen >= TOKEN_LEN + UUID_LEN + PASSWORD_LEN) {
        std::string token(loginInfo->token,TOKEN_LEN);
        int pos = token.find_first_of(' ',0);
		while(string::npos != pos)
		{
			token.erase(pos,1);
			pos = token.find_first_of(' ',0);
		}
		std::transform(token.begin(),token.end(),token.begin(),(int(*)(int))toupper);
        if( token.length() == TOKEN_LEN ) {
            WisIOSTokenDao::save( loginInfo->uuid, token );
        } else {
            LOG_ERROR("user(uuid = %s) token(token = %s) is illegal",std::string(loginInfo->uuid,UUID_LEN).c_str(),std::string(loginInfo->token,TOKEN_LEN).c_str());
        }
    }
    sendLoginResponse( busAddress, loginInfo->uuid,0,TYPE_USER );
	TRACE_OUT();
}
void WisLoginHandler::handleUserLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid)
{
   TRACE_IN();
   WisUserDao::logout(uuid, WisUserDao::defaultPassword);
   mapDeleteUser(busAddress);
   WisLogDao::saveUserLogoutLog(uuid, 0, "");
  TRACE_OUT();
}

void WisLoginHandler::handleKickoutUser(std::string uuid)
{
	TRACE_IN();
	BUS_ADDRESS_POINTER busAddress = getUserAddress(uuid);
	mapDeleteUser(busAddress);
	if(busAddress != NULL)
		GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_SERVICE_KICKOUT_USER,NULL,0,TCP_SERVER_MODE);	
	TRACE_OUT();
}


void WisLoginHandler::handleDeviceLogin( BUS_ADDRESS_POINTER busAddress,int datalen,char *pdata )
{
	TRACE_IN();
    WisDeviceLoginInfo* loginInfo = (WisDeviceLoginInfo*)pdata;
	
    bool firstLogin = false;
    bool rc = WisDeviceDao::check(std::string(loginInfo->uuid,UUID_LEN));
    if( !rc ) {
        rc = WisDeviceDao::regist(std::string(loginInfo->uuid,UUID_LEN), std::string(loginInfo->name,DEVICE_NAME_LEN));
        if( !rc ) {
            LOG_ERROR("device(uuid = %s,name = %s )regist failed.",std::string(loginInfo->uuid,UUID_LEN).c_str(),
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
	mapAddDevice(busAddress,std::string(loginInfo->uuid));
    sendLoginResponse( busAddress,loginInfo->uuid, 1 ,TYPE_DEVICE);	
    
    std:map<std::string,WisUserInfo> mapUserIfno;
	if(0 < WisBindDao::getBindedUsers(loginInfo->uuid,mapUserIfno))
	{
		for(std::map<std::string,WisUserInfo>::iterator ite = mapUserIfno.begin();\
			ite != mapUserIfno.end();ite++)
			{
			    BUS_ADDRESS_POINTER pBus_address = getUserAddress(std::string(ite->second.uuid));
				if(pBus_address != NULL)
					GetUniteDataModuleInstance()->SendData(pBus_address,WIS_CMD_USER_DEV_LOGIN,loginInfo->uuid,\
					strlen(loginInfo->uuid),TCP_SERVER_MODE);
			}			
		LOG_INFO("device(uuid = %s) login,send %d notifications to online user",loginInfo->uuid,(int )mapUserIfno.size());
		
	}	
	TRACE_OUT();
}

void WisLoginHandler::handleDeviceLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid )
{
	TRACE_IN();
    WisDeviceDao::logout( uuid );
	mapDeleteDevice(busAddress);
    WisLogDao::saveDeviceLogoutLog(uuid, 0, "");
	
    std:map<std::string,WisUserInfo> mapUserIfno;
	if(0 < WisBindDao::getBindedUsers(uuid,mapUserIfno))
	{
		for(std::map<std::string,WisUserInfo>::iterator ite = mapUserIfno.begin();\
			ite != mapUserIfno.end();ite++)
			{
				BUS_ADDRESS_POINTER pBus_address = getUserAddress(std::string(ite->second.uuid));
				if(pBus_address != NULL)
					GetUniteDataModuleInstance()->SendData(pBus_address,WIS_CMD_USER_DEV_LOGOUT,(char *)uuid.c_str(),\
					strlen(uuid.c_str()),TCP_SERVER_MODE);
			}			
		LOG_INFO("device(uuid = %s) logout,send %d notifications to online user",uuid.c_str(),(int)mapUserIfno.size());
		
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