


#ifndef  MAIL_H_
#define MAIL_H_

#include "quickmail.h"
#include <string>
#include <mutex>
#include <set>

using namespace std;
class Mail{
public:
	Mail();
	~Mail(){if(NULL != mailObj) quickmail_destroy(mailObj);}
	bool sendMail(const string& content) const;
	bool getInitParam();
	bool addTo(const string &uuid);
	bool addCc(const string &uuid);
	bool addBcc(const string &uuid);
	bool addAttachment(const string &file);
	
	
private:
	static string _from;
	static string _user;
	static string _password;
	static string _subject;
	static string _smtpServer;
	static short _port;
	
	static std::mutex gMutex;
	static bool isQuickMail_inited;

	quickmail mailObj;
};
#endif