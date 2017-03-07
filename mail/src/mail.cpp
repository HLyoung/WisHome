
#include "mail.h"
#include "UniteDataModule.h"
#include "tinyxml.h"
#include <iostream>

std::mutex Mail::gMutex;
bool Mail::isQuickMail_inited = false;
string Mail::_from = "";
string Mail::_user = "";
string Mail::_password = "";
string Mail::_subject = "";
string Mail::_smtpServer = "";
short Mail::_port = 25;

Mail::Mail()
{	
	gMutex.lock();
	if(!isQuickMail_inited){
		quickmail_initialize();
		if(getInitParam())
			isQuickMail_inited = true;
	}
	gMutex.unlock();
	mailObj = quickmail_create(_from.c_str(),_subject.c_str()); 
}
bool Mail::sendMail(const string& content) const 
{	
	if(isQuickMail_inited && NULL != mailObj){
		quickmail_add_header(mailObj, "Importance: Low");
	    quickmail_add_header(mailObj, "X-Priority: 5");
	    quickmail_add_header(mailObj, "X-MSMail-Priority: Low");
		quickmail_add_body_memory(mailObj, "text/html",(char *)(content.c_str()), content.length(), 0);
	 
		const char*errmsg = quickmail_send(mailObj,_smtpServer.c_str(),_port,_user.c_str(),_password.c_str());
		if(NULL == errmsg)
			return true;
		}
	return false;
}

bool Mail::getInitParam(){
	CXmlDocument xmlDocument;
	CUniteDataModule::GetInstance()->GetConfigXml(xmlDocument);
	CXmlElement *pRootElement = xmlDocument.RootElement();
	if(0 != pRootElement){
		CXmlElement *pMailElement = pRootElement->FirstChildElement("mail");
		if(0 != pMailElement){
			_from = pMailElement->Attribute("from");
			_user = pMailElement->Attribute("user");
			_password = pMailElement->Attribute("password");
			_subject = pMailElement->Attribute("subject");
			_smtpServer = pMailElement->Attribute("smtpServer");
			_port = atoi(pMailElement->Attribute("port"));
			return true;
			}
		}
	return false;
}

bool Mail::addTo(const string & uuid){
	if(isQuickMail_inited && NULL != mailObj){
		quickmail_add_to(mailObj,uuid.c_str());
		return true;
	}else 
		return false;
}

bool Mail::addCc(const string & uuid){
	if(isQuickMail_inited && NULL != mailObj){
		quickmail_add_cc(mailObj,uuid.c_str());
		return true;
	}else
		return false;
}

bool Mail::addBcc(const string & uuid){
	if(isQuickMail_inited && NULL != mailObj){
		quickmail_add_bcc(mailObj,uuid.c_str());
		return true;
	}else	
		return false;
}

bool Mail::addAttachment(const string & file){
	if(isQuickMail_inited && NULL != mailObj){
		quickmail_add_attachment_file(mailObj,file.c_str(),NULL);
		return true;
	}else
		return false;
}

/*int main()
{
	string content = string("dear sir:<br />&nbsp&nbsp&nbsp&nbsp please click this link <a href=\"http://192.168.2.70/test.php\" id=\"myid\">192.168.2.70/test.php</a> to reset your password \
		<a href=\"https://baidu.com\">qubaidu</a>"); 	
	Mail mail;
	mail.addTo("1255324511@qq.com");
	if(mail.sendMail(content)){
		std::cout<<"OK"<<std::endl;
		return -1;
	}
	std::cout<<"ERROR"<<std::endl;
	return 0;
}*/
