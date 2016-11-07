

#ifndef JPUSH_H_
#define JPUSH_H_

#include "json/json.h"
#include "curlWrapper.h"
#include <string>
#include <set>

typedef enum {
	ios,
	android,
	all
}DEVICETYPE;

typedef enum {
	ALL,
	TAG,
	TAG_AND,
	ALIAS,
	SEGMENT,
	REGISTRATION_ID
}AUDICETYPE;

class Cplatform{
	
public:
	Cplatform(DEVICETYPE type){type_ = type;}	
	void  toJson(Json::Value &jsonValue);
private:
	DEVICETYPE type_;

};

class Caudience{
public:
	Caudience(AUDICETYPE type,std::set<std::string> values){
		this->type_ = type;
		this->values_ = values;
	}
	void toJson(Json::Value &jsonValue);
private:
	AUDICETYPE type_;
	std::set<std::string> values_;
};

class Cnotification{
public:	
	 Cnotification(std::string alert,std::string title,int build_id,Json::Value extras);
	 void toJson(Json::Value &jsonValue);
private:
	std::string alert_;
	std::string title_;
	int build_id_;
	Json::Value extras_;
};

class Cmessage{
public:
	Cmessage(std::string msg_content,std::string title,std::string content_type,Json::Value extras);
	void toJson(Json::Value &jsonValue);
private:
	std::string msg_content_;
	std::string title_;
	std::string content_type_;
	Json::Value extras_;
};

class Csms{
public:
	Csms(std::string content,int delay);
	void toJson(Json::Value &jsonValue);
private:
	std::string content_;
	int delay_;
};

class Coptions{  //暂时整一个参数
public:
	Coptions(bool production){isProduction_ = production;}
	void toJson(Json::Value &jsonValue);
private:
	bool isProduction_;
};


class CPushPayload{
public:
	CPushPayload();
	Json::Value toJson();
	void setPlatform(Cplatform *platform){platform_ = platform;}
	void setAudience(Caudience *audience){audience_ = audience;}
	void setMessage(Cmessage *message){message_ = message;}
	void setNotification(Cnotification *notification){notification_ = notification;}
	void setOptions(Coptions *options){options_ = options;}
	void setSms(Csms *sms){sms_ = sms;}
private:
    Cplatform *platform_;
	Caudience *audience_;
	Cmessage *message_;
	Cnotification *notification_;
	Coptions *options_;
	Csms *sms_;
	Json::Value jsonValue;
	
};

class JPush{
public:
	static CURLcode push_ALL_ALL_Alert(std::string alert,std::string title,int build_id);
	static CURLcode push_SpecifiedIDs(std::string alert,std::string title,int build_id,std::set<std::string>& ids);
};
#endif //JPUSH_H_