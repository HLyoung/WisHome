

#include "wisLogginHandler.h"

std::mutex WisLoginHandler::uMutex;
std::mutex WisLoginHandler::dMutex;
std::map<BUS_ADDRESS_POINTER,std::string> WisLoginHandler::mUser;
std::map<BUS_ADDRESS_POINTER,std::string> WisLoginHandler::mDevice;

void WisLoginHandler::sendLoginResponse(BUS_ADDRESS_POINTER busAddress,const char *m_uuid,int done,int type )
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
	std::string token = getTokenFromBuffer(loginInfo->token);
	
    if(WisUserDao::login(uuid, password)){
		mapAddUser(busAddress,uuid);
	    WisLogDao::saveUserLoginLog(uuid, strlen((char *)busAddress->host_address.ip),(const char*)busAddress->host_address.ip);
	    if ( datalen >= TOKEN_LEN + UUID_LEN + PASSWORD_LEN) {
			std::transform(token.begin(),token.end(),token.begin(),(int(*)(int))toupper);
			if(JIGUANG_TOKEN_LEN == token.length())
				WisIOSTokenDao::save(uuid, token );
			else
				LOG_INFO("ILLEGAL TOKEN: useID=%s,token=%s,ip=%s,port=%d",uuid.c_str(),token.c_str(),busAddress->host_address.ip,busAddress->host_address.port);
	    	}
		LOG_INFO("USER LOGIN SUCCESS: useID = %s,ip=%s,port=%d",uuid.c_str(),busAddress->host_address.ip,busAddress->host_address.port);
	    sendLoginResponse( busAddress, loginInfo->uuid,0,TYPE_USER );
		return;
    	}
	LOG_INFO("USER LOGIN FAILED: useID = %s,ip=%s,port=%d",uuid.c_str(),busAddress->host_address.ip,busAddress->host_address.port);
	sendLoginResponse( busAddress, loginInfo->uuid,-1,TYPE_USER );
	TRACE_OUT();
}
void WisLoginHandler::handleUserLogout(BUS_ADDRESS_POINTER busAddress,std::string uuid)
{
   if(WisUserDao::logout(uuid, WisUserDao::defaultPassword)){
   		mapDeleteSameUser(uuid);
  		WisLogDao::saveUserLogoutLog(uuid, 0, "");
		LOG_INFO("USER LOGOUT SUCCESS: %s",uuid.c_str());
   	    }
   else
   	   LOG_INFO("USER LOGOUT FAILED: %s",uuid.c_str());
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
	string sUuid = getUuidFromBuffer(loginInfo->uuid);
	string sName = std::string(loginInfo->name,UUID_LEN);
	
    bool firstLogin = false;
    bool rc = WisDeviceDao::check(sUuid.c_str());
    if( !rc ) {
        rc = WisDeviceDao::regist(sUuid.c_str(), sName);
        if( !rc ) {
            LOG_ERROR("DEVICE LOGIN FALIED: uuid = %s,name = %s.",sUuid.c_str(),
            sName.c_str());
            sendLoginResponse( busAddress,sUuid.c_str(), -1,TYPE_DEVICE );
            return;
        }
        firstLogin = true;
    }
    rc = WisDeviceDao::login(sUuid.c_str(),sName.c_str());
    if( !rc ) {
        LOG_ERROR("DEVICE LOGIN FAILED: uuid = %s,name = %s.",sUuid.c_str(),sName.c_str());
        sendLoginResponse( busAddress,sUuid.c_str(),-1,TYPE_DEVICE );
        return;
    }
    if( firstLogin ) {
        WisLogDao::saveDeviceRegLog(sUuid.c_str(), strlen((char *)busAddress->host_address.ip), (const char *)busAddress->host_address.ip);
    } else {
        WisLogDao::saveDeviceLoginLog(sUuid.c_str(), strlen((char*)busAddress->host_address.ip),(const char*) busAddress->host_address.ip);
    } 
	mapAddDevice(busAddress,sUuid);
	LOG_INFO("DEVICE LOGIN SUCCESS: %s",sUuid.c_str());
    sendLoginResponse( busAddress,sUuid.c_str(), 1 ,TYPE_DEVICE);	
    
    std:map<std::string,WisUserInfo> mapUserIfno;
	if(0 < WisBindDao::getBindedUsers(sUuid.c_str(),mapUserIfno))
	{
		for(std::map<std::string,WisUserInfo>::iterator ite = mapUserIfno.begin();\
			ite != mapUserIfno.end();ite++)
			{
			    BUS_ADDRESS_POINTER pBus_address = getUserAddress(sUuid);
				if(pBus_address != NULL)
					GetUniteDataModuleInstance()->SendData(pBus_address,WIS_CMD_USER_DEV_LOGIN,(char *)(sUuid.c_str()),sUuid.length(),TCP_SERVER_MODE);
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
		LOG_INFO("DEVICE LOGOUT SUCCESS: %s",uuid.c_str());

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
	TRACE_IN();
	std::lock_guard<std::mutex> lg(uMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mUser.begin();
	for(;ite != mUser.end();){
		if(ite->second == uuid  && bus_address != ite->first){		//kick out other user with the same id.	
			GetUniteDataModuleInstance()->SendData(ite->first,WIS_CMD_SERVICE_KICKOUT_USER,NULL,0,TCP_SERVER_MODE);	
			std::map<BUS_ADDRESS_POINTER,std::string>::iterator ote = ite++;
			mUser.erase(ote);
			LOG_INFO("KICKOUT USER: useID=%s,ip=%s,port=%d",uuid.c_str(),bus_address->host_address.ip,bus_address->host_address.port);
			continue;
			}
		ite++;
		}		
	mUser[bus_address] = uuid;
	TRACE_OUT();
}

void WisLoginHandler::mapAddDevice(BUS_ADDRESS_POINTER bus_address, const std::string & uuid)
{
	TRACE_IN();
	std::lock_guard<std::mutex> lg(dMutex);
	std::map<BUS_ADDRESS_POINTER,std::string>::iterator ite = mDevice.begin();
	for(;ite != mDevice.end();){
		if(ite->second == uuid && bus_address != ite->first){
			std::map<BUS_ADDRESS_POINTER,std::string>::iterator ote = ite++;
			LOG_INFO("KICKOUT DEVICE: useID=%s,ip=%s,port=%d",uuid.c_str(),bus_address->host_address.ip,bus_address->host_address.port);
			mDevice.erase(ote);
			continue;
			}
		ite++;
		}
	mDevice[bus_address] = uuid;
	TRACE_OUT();
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
		if(ite->second == uuid){
			std::map<BUS_ADDRESS_POINTER,std::string>::iterator ote = ite++;
			mUser.erase(ote);
			continue;
			}
		ite++;
		}	
}


void WisLoginHandler::mapDeleteDevice(BUS_ADDRESS_POINTER bus_address)
{
	std::lock_guard<std::mutex> lg(dMutex);
	if(mDevice.find(bus_address) != mDevice.end())
		mDevice.erase(bus_address);
}