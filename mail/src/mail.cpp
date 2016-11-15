
#include "mail.h"
#include <iostream>

const std::string user = "1017348166@qq.com";
const std::string password = "syxqcpwusgnkbeaf";
const std::string from = "1017348166@qq.com";
const std::string subject = "wisHome account reset password ";
const std::string smtpServer = "smtp.qq.com";
short port = 25;

std::mutex Mail::gMutex;
bool Mail::isQuickMail_inited = false;

Mail::Mail()
{	
	gMutex.lock();
	if(!isQuickMail_inited){
		quickmail_initialize();
		isQuickMail_inited = true;
	}
	gMutex.unlock();
	_isObjInited = false;	
}

void Mail::init(string from,string user,string password,string subject,short port)
{
	_from = from;
	_user = user;
	_password = password;
	_subject = subject;
	_port = port;
	_isObjInited = true;
}

bool Mail::setTargetAndFile(std::set<string> &to,std::set<string> &cc,std::set<string> &bcc,std::set<string> &file)
{
	_setTo.clear();
	_setCc.clear();
	_setBcc.clear();
	_setAttachmentFile.clear();
	
	std::set<string>::iterator ite= to.begin();
	int n = ite->find('@',0);
	if(n == string::npos)
		return false;
	_smtpServer = "smtp." + ite->substr(n + 1,ite->length());
	
	for(; ite != to.end();ite++ )
		_setTo.insert(*ite);
	
	ite = cc.begin();
	for(; ite != cc.end();ite++)
		_setCc.insert(*ite);
	
	ite = bcc.begin();
	for(; ite != bcc.end();ite++)
		_setBcc.insert(*ite);
	
	ite = file.begin();
	for(; ite != file.end();ite++)
		_setAttachmentFile.insert(*ite);
	
	return true;
}
bool Mail::sendMail(const string &content)
{
	quickmail mailobj = NULL;
	if(_isObjInited)
		mailobj = quickmail_create(_from.c_str(),_subject.c_str()); 
	else
		mailobj = quickmail_create(from.c_str(),subject.c_str());
	
	std::set<string>::iterator ite = _setTo.begin();
	for(; ite!= _setTo.end();ite++)
		quickmail_add_to(mailobj,(*ite).c_str());
	
	ite = _setCc.begin();
	for(; ite!= _setCc.end();ite++)
		quickmail_add_cc(mailobj,(*ite).c_str());
	
	ite = _setBcc.begin();
	for(;ite != _setBcc.end();ite ++)
		quickmail_add_bcc(mailobj,(*ite).c_str());
	
	ite = _setAttachmentFile.begin();
	for(; ite != _setAttachmentFile.end();ite++)
		quickmail_add_attachment_file(mailobj,ite->c_str(),NULL);
	
	quickmail_add_header(mailobj, "Importance: Low");
    quickmail_add_header(mailobj, "X-Priority: 5");
    quickmail_add_header(mailobj, "X-MSMail-Priority: Low");
	quickmail_add_body_memory(mailobj, "text/html",(char *)(content.c_str()), content.length(), 0);
    
	const char* errmsg;
	if(_isObjInited)
		errmsg = quickmail_send(mailobj,_smtpServer.c_str(),_port,_user.c_str(),_password.c_str());
	else
		errmsg = quickmail_send(mailobj,smtpServer.c_str(),port,user.c_str(),password.c_str());
    quickmail_destroy(mailobj);
	
	if(NULL == errmsg)
		return true;
	else
		return false;
}

/*int main()
{
	string content = string("dear sir:<br />&nbsp&nbsp&nbsp&nbsp please click this link <a href=\"http://192.168.2.70/test.php\" id=\"myid\">192.168.2.70/test.php</a> to reset your password \
		<a href=\"https://baidu.com\">qubaidu</a>"); 	
    std::set<std::string> to,cc,bcc,file;
	to.insert("sfafasdfasdfdasfsdfsdfsdfsdafsdafsdafdsfdsfasdfasdfsd@163.com");
	Mail mail;
	mail.setTargetAndFile(to,cc,bcc,file);
	if(mail.sendMail(content)){
		std::cout<<"OK"<<std::endl;
		return -1;
	}
	std::cout<<"ERROR"<<std::endl;
	return 0;
}*/
