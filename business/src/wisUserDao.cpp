#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



#include "wisUserDao.h"
#include "libSql.h"
#include "CNVDataAccess.h"
#include "WisBindDao.h"
#include "wisLogginHandler.h"

std::string WisUserDao::defaultPassword = "R800JL.Ke8MBo";

bool WisUserDao::checkUser(const std::string &user)
{
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"select * from wis_user_tbl where `name`='%s'",user.c_str());
		if(access->ExecuteNoThrow(sql) >= 1){
			DbaModule_ReleaseNVDataAccess(access);
			return true;
			}
		DbaModule_ReleaseNVDataAccess(access);
		}
	return false;	
}

bool WisUserDao::checkUserAndPassword(const std::string &user,const std::string &password){
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"select * from wis_user_tbl where `name`='%s' and `password`='%s'",user.c_str(),password.c_str());
		if(access->ExecuteNoThrow(sql) >= 1){
			DbaModule_ReleaseNVDataAccess(access);
			return true;
			}
		DbaModule_ReleaseNVDataAccess(access);
		}
	return false;	
}

bool WisUserDao::login(const std::string &user,const std::string& passwd)
{
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();

	bool ret = false;
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"select * from wis_user_tbl where `name`='%s' and `password`='%s'",user.c_str(),passwd.c_str());
		if(access->ExecuteNoThrow(sql) >= 1){
		    access->FreeResult();
			snprintf(sql,sizeof(sql),"update wis_user_tbl set `login_time`=CURRENT_TIMESTAMP,`login_cnt`=`login_cnt`+1,`status`=1 where `name`='%s'",user.c_str());
			if(-1 != access->ExecuteNonQuery(sql))
				ret = true;
			else
				LOG_INFO("user(uuid = %s password = %s) loggin failed",user.c_str(),passwd.c_str());
			}
		else
			LOG_INFO("user(uuid = %s password = %s) does`t exit!",user.c_str(),passwd.c_str());
		DbaModule_ReleaseNVDataAccess(access);
		}
	return ret;
}


bool WisUserDao::logout(const std::string &user,const std::string &passwd)
{
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL !=  access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"update wis_user_tbl set `status`=0 where `name`='%s'",user.c_str());
		if(-1 != access->ExecuteNonQuery(sql)){
			DbaModule_ReleaseNVDataAccess(access);
			return true;
			}
		DbaModule_ReleaseNVDataAccess(access);	
		}	
	return false;
}


bool WisUserDao::regist(const std::string &user,const std::string &passwd)
{
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"insert into wis_user_tbl(`name`,`password`,`type`,`permission`,\
		`reg_time`,`login_cnt`,`bind_status`) values('%s','%s', 0, 0, CURRENT_TIMESTAMP,0,0)",user.c_str(), passwd.c_str());
		if(-1 != access->ExecuteNonQuery(sql)){
			DbaModule_ReleaseNVDataAccess(access);
			return true;
		}
		DbaModule_ReleaseNVDataAccess(access);
		}
	return false;
}

bool WisUserDao::logoutAll()
{
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		const char *sql = "update wis_user_tbl set `status`=0";
		if(-1 != access->ExecuteNonQuery(sql)){
			DbaModule_ReleaseNVDataAccess(access);
			return true;
		}
		DbaModule_ReleaseNVDataAccess(access);
		}
	return false;
	
}

bool WisUserDao::userGetDevice(BUS_ADDRESS_POINTER busAddress,const char *uuid)
{
	std::map<std::string,WisDeviceInfo> mapDevice;
	int count = WisBindDao::getBindedDevices(getUuidFromBuffer(uuid),mapDevice);
	int size = sizeof(int) + count*sizeof(WisDeviceInfo);

	WisUserDeviceList *deviceList = (WisUserDeviceList*)malloc(size);
	if(deviceList){
		deviceList->nums = count;
		int index = 0;
		std::map<std::string,WisDeviceInfo>::iterator iter = mapDevice.begin();
		for(;iter != mapDevice.end();iter++,index++){
			memcpy((char *)deviceList + sizeof(int) + index*sizeof(WisDeviceInfo),&(iter->second),sizeof(WisDeviceInfo));
			}
		}
	GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_USER_GET_DEVICES,(char *)deviceList,size,TCP_SERVER_MODE);
	LOG_INFO("GET DEVICE SUCCESS: %s",uuid);
	SafeDeleteArray(deviceList);
}

int WisUserDao::handleUserRegist(BUS_ADDRESS_POINTER busAddress,const char * pdata)
{
	WisUserRegistInfo *registInfo = (WisUserRegistInfo*)pdata;	
	int ret = 0;
    if(checkUser(getUuidFromBuffer(registInfo->uuid))) ret = -2	;
	else{
		CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
		if(NULL != access){
			char sql[200] = {0};
			snprintf(sql,sizeof(sql),"insert into wis_user_tbl(`name`,`password`,`type`,`permission`,\
			`reg_time`,`login_cnt`) values('%s','%s', 0, 0, CURRENT_TIMESTAMP,0)",getUuidFromBuffer(registInfo->uuid).c_str(),getPasswordFromBuffer(registInfo->password).c_str());
		    if(-1 != access->ExecuteNonQuery(sql)){
				LOG_INFO("USER REGIST: %s",getUuidFromBuffer(registInfo->uuid).c_str());
				ret = 0; 
		    	}
			else ret = -1;
			DbaModule_ReleaseNVDataAccess(access);
			}
		}
	sendUserResponse(busAddress,WIS_CMD_USER_REGIST,ret);
	return ret;
}

void WisUserDao::handleUserModifyPassword(std::string &uuid,BUS_ADDRESS_POINTER busAddress,const char *pdata)
{
	WisUserModifyPassword *modifyInfo  = (WisUserModifyPassword*)pdata;
    CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"update wis_user_tbl set `password`='%s' where `name`='%s'",string(modifyInfo->newPassword).c_str(),uuid.c_str());
		if(-1 != access->ExecuteNonQuery(sql)){
			sendUserResponse(busAddress,WIS_CMD_USER_MODIFY_PASSWORD,0);
			DbaModule_ReleaseNVDataAccess(access);
			LOG_INFO("USER MODIFY PASSWORD: %s ",uuid.c_str());
			return ;
			}
		}
	sendUserResponse(busAddress,WIS_CMD_USER_MODIFY_PASSWORD,-1);
	DbaModule_ReleaseNVDataAccess(access);
}

void WisUserDao::handleUserResetPassword(BUS_ADDRESS_POINTER busAddress,const char *pdata)
{
	WisUserResetPassword*resetInfo = (WisUserResetPassword*)pdata;	
    if(!checkUser(getUuidFromBuffer(resetInfo->uuid))){
		LOG_INFO("user(uuid = %s) want to reset his password,but he is not registed yet",string(resetInfo->uuid).c_str());
		sendUserResponse(busAddress,WIS_CMD_USER_RESET_PASSWORD,-2);
		return;
    }	
	if(sendResetPasswordMailTo(getUuidFromBuffer(resetInfo->uuid)))
		sendUserResponse(busAddress,WIS_CMD_USER_RESET_PASSWORD,0);
	else
		sendUserResponse(busAddress,WIS_CMD_USER_RESET_PASSWORD,-1);

	LOG_INFO("USER RESET PASSWORD: %s",getUuidFromBuffer(resetInfo->uuid).c_str());
}

string WisUserDao::makeupURL(const std::string& uuid)
{
	return "wisHome password manager";
}

bool WisUserDao::sendUserResponse(BUS_ADDRESS_POINTER  busAddress,int cmd,const int ret)
{
	return GetUniteDataModuleInstance()->SendData(busAddress,cmd,(char *)&ret,sizeof(int),TCP_SERVER_MODE);
}


bool WisUserDao::sendResetPasswordMailTo(const std::string & uuid)
{
	string url = makeupURL(string(uuid));
	string content = string("dear sir:<br/> &nbsp &nbsp &nbsp &nbsp &nbsp  please click this link <a href=\"http://192.168.2.70/test.php\">") + url + string(" </a>to reset your password"); 	

	Mail mail;
	std::set<std::string> to,cc,bcc,file;
	to.insert(uuid);
	mail.setTargetAndFile(to,cc,bcc,file);
	if(!mail.sendMail(content))
		return false;
	else
		return true;
}

bool WisUserDao::sendGreetMailTo(const std::string & uuid)
{
	string content = string("Dear sir:<br/> &nbsp &nbsp &nbsp &nbsp &nbsp  thanks for register wishome. ");	
    std::set<std::string> to,cc,bcc,file;
	to.insert(uuid);
	Mail mail;
    mail.setTargetAndFile(to,cc,bcc,file);
    if(!mail.sendMail(content))
		return false;
	else
		return true;	
}

bool WisUserDao::checkMail(const std::string &mail)
{
	int n = mail.find('@',0);
	if(n == string::npos)
		return false;
	std::string _smtpServer = "smtp." + mail.substr(n + 1,mail.length());

	hostent *ph = gethostbyname(_smtpServer.c_str());
    if( ph == NULL ) {
        return false;
    }
    
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(25);    //port of SMTP 
    memcpy(&sin.sin_addr, (in_addr*)ph->h_addr_list[0], ph->h_length);
   
	unsigned long ul = 1;  
	int s = socket(PF_INET, SOCK_STREAM, 0);
	//fcntl(s, F_SETFL, O_NONBLOCK);
    if(connect(s, (sockaddr*)&sin, sizeof(sin)) ) 
       return false;
	close(s);
	//SafeFree(ph);
   	return true;
}

bool WisUserDao::handleUserQuit(BUS_ADDRESS_POINTER busAddress,const char *pdata)
{
	WisUserQuitInfo *quitinfo = (WisUserQuitInfo *)pdata;
	CNVDataAccess *access = (CNVDataAccess *)DbaModule_GetNVDataAccess();
	if(NULL != access){
		char sql[200] = {0};
		snprintf(sql,sizeof(sql),"delete from wis_ios_token_tbl2 where `token`='%s'",quitinfo->token);
		if(-1 != access->ExecuteNonQuery(sql)){
			sendUserResponse(busAddress,WIS_CMD_USER_QUIT,0);
			WisLoginHandler::handleUserLogout(busAddress,getUuidFromBuffer(quitinfo->uuid));
			DbaModule_ReleaseNVDataAccess(access);
			LOG_INFO("USER QUIT: %s",getUuidFromBuffer(quitinfo->uuid).c_str());
			return  true;
			}
		DbaModule_ReleaseNVDataAccess(access);
		}
	sendUserResponse(busAddress,WIS_CMD_USER_QUIT,-1);	
	return false;
}




