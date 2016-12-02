

#ifndef _WIS_USER_DAO_H_
#define _WIS_USER_DAO_H_

#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <condition_variable>
#include "libSql.h"
#include "libLog.h"
#include "CNVDataAccess.h"
#include "IUniteDataModule.h"
#include "wis.h"
#include "mail.h"
#include "encrypt.h"

class WisUserDao{
public:
	static bool checkUser(const std::string &user);
	static bool checkUserAndPassword(const std::string &user,const std::string &password);
	static bool login(const std::string &user,const std::string &passwd="");
	static bool logout(const std::string &user,const std::string &passwd="");
	static bool regist(const std::string &user,const std::string &passwd=defaultPassword);
	static bool logoutAll();
	static bool userGetDevice(BUS_ADDRESS &busAddress,const char *uuid);
	static bool sendUserResponse(BUS_ADDRESS &busAddress,int cmd,const int ret);

	static void handleUserRegist(BUS_ADDRESS &busAddress,const char *pdata);
	static void handleUserResetPassword(BUS_ADDRESS &busAddress,const char *pdata);
	static void handleUserModifyPassword(std::string &uuid,BUS_ADDRESS &busAddress,const char *pdata);
	static bool sendResetPasswordMailTo(const std::string &emailAddress);
	static bool sendGreetMailTo(const std::string &uuid);
	static string makeupURL(const std::string &uuid);
	static bool checkMail(const std::string &mail);
	static bool handleUserQuit(BUS_ADDRESS& busAddress,const char *pdata);
public:
	static std::string defaultPassword;
};


#endif