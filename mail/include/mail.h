


#ifndef  MAIL_H_
#define MAIL_H_

#include "quickmail.h"
#include <string>
#include <mutex>
#include <set>

using namespace std;
const std::string content = "Dear sir: 	you verification code is 110";

class Mail{
public:
	Mail();
	~Mail(){}
	void init(string from, string user,string password,string subject,short port);
	bool sendMail(const string& content = content);
	bool setTargetAndFile(std::set<string> &to,std::set<string> &cc,std::set<string> &bcc,std::set<string> &file);
	
	
private:
	string _from;
	string _user;
	string _password;
	string _subject;
	string _smtpServer;
	short _port;
	bool _isObjInited;
	
	static std::mutex gMutex;
	static bool isQuickMail_inited;
	
	std::set<std::string> _setTo;
	std::set<std::string> _setCc;
	std::set<std::string> _setBcc;
	std::set<std::string> _setAttachmentFile;	
};
#endif