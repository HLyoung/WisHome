
#include "JPush.h"
#include "libLog.h"
#include "string.h"
//using namespace std;

void Cplatform::toJson(Json::Value &jsonValue){
		switch(type_)
		{
			case ios:
				jsonValue["platform"] = "ios";
				break;
			case android:
				jsonValue["platform"] = "android";
				break;
			case all:
				jsonValue["platform"] = "all";
				break;
			default:
				break;			
		}
}

void Caudience::toJson(Json::Value &jsonValue){
	if(type_ == ALL)
	{
		jsonValue["audience"] = "all";
		return;
	}

	int i = 0;
	Json::Value value;
	for(std::string str:values_)
	{
		//Json::Value strValue;
		//strValue[str.c_str()] = "";
		//value.append(strValue);
		value[i] = str;
		i++;
	}
	
	Json::Value typeValue;
	switch(type_)
	{
		case TAG:
			typeValue["tag"] = value;
			break;
		case TAG_AND:
			typeValue["tab_and"] = value;
			break;
		case ALIAS:
			typeValue["alias"] = value;
			break;
		case SEGMENT:
			typeValue["segment"] = value;
			break;
		case REGISTRATION_ID:
			typeValue["registration_id"] = value;
			break;
		default:
			break;
	}
	
	jsonValue["audience"] = typeValue;
}

Cnotification::Cnotification(std::string alert,std::string title,int build_id,Json::Value extras){
	alert_ = alert;
	title_ = title;
	build_id_ = build_id;
	extras_ = extras;
}


void Cnotification::toJson(Json::Value &jsonValue){

	Json::Value allValue;
	
	char badge[10] = {0};
	snprintf(badge+1,sizeof(badge),"%d",build_id_);	
	badge[0] = '+';
	Json::Value iosValueDetail;
	
	allValue["alert"] = alert_.c_str();
	iosValueDetail["badge"] = badge;
	allValue["ios"] = iosValueDetail;
	
	Json::Value androidValueDetail;
	androidValueDetail["title"] = title_.c_str();
	androidValueDetail["build_id"] = build_id_;

	allValue["android"] = androidValueDetail;
	
	jsonValue["notification"] = allValue;
}

Cmessage::Cmessage(std::string msg_content,std::string title,std::string content_type,Json::Value extras){
	msg_content_ = msg_content;
	title_ = title;
	content_type_ = content_type;
	extras_ = extras;
}


void Cmessage::toJson(Json::Value &jsonValue){
	Json::Value allValue;
	allValue["msg_content"] = msg_content_.c_str();
	allValue["title"] = title_.c_str();
	allValue["content_type_"] = content_type_.c_str();
	allValue["extras"] = extras_;
	
	jsonValue["message"] = allValue;
}

Csms::Csms(std::string content,int delay){
	content_ = content;
	delay_ = delay;
}


void Csms::toJson(Json::Value &jsonValue){
	Json::Value allValue;
	allValue["content"] = content_;
	allValue["delay_time"] = delay_;
	
	jsonValue["sms_message"] = allValue;
}

void  Coptions::toJson(Json::Value &jsonValue){
	Json::Value allValue;
	if(isProduction_)
		allValue["apns_production"] = "True";
	else
		allValue["apns_production"] = "False";
	
	jsonValue["options"] = allValue;
}


CPushPayload::CPushPayload(){
	platform_ = NULL;
	audience_ = NULL;
	message_ = NULL;
	notification_ = NULL;
	options_ = NULL;
	sms_ = NULL;
}

Json::Value CPushPayload::toJson(){
    if(options_ != NULL)
	   options_->toJson(jsonValue);
    if(sms_ != NULL)
	   sms_->toJson(jsonValue);
    if(message_ != NULL)
	   message_->toJson(jsonValue);
    if(notification_ != NULL)
		notification_->toJson(jsonValue);	
    if(audience_ != NULL)
		audience_->toJson(jsonValue);
	if(platform_ != NULL)
		platform_->toJson(jsonValue);
		
	return jsonValue;
}

CURLcode JPush::push_ALL_ALL_Alert(std::string alert,std::string title,int build_id){
	TRACE_IN();
	CPushPayload payload;
	
	Json::Value test;
	std::set<std::string> tmpSet;;
	Cplatform platform(all);
	Caudience audience(ALL,tmpSet);
	Cnotification notification(alert,title,build_id,test);
	payload.setPlatform(&platform);
	payload.setAudience(&audience);
	payload.setNotification(&notification);
	
	CcurlHandle *curlHandle  = CcurlModule::curlGetHandle();
	Json::FastWriter writer;
	std::string tmp3 = writer.write(payload.toJson());
	std::string targetFileds = std::string(tmp3.c_str(),tmp3.length()-1);
	curlHandle->setOption(CURLOPT_POSTFIELDS,targetFileds.c_str());

	CURLcode ret = curlHandle->perform();
	CcurlModule::curlRelessHandle(curlHandle);
	
	return ret;
	TRACE_OUT();
}

CURLcode JPush::push_SpecifiedIDs(std::string alert,std::string title,int build_id,std::set<std::string>& ids)
{
	TRACE_IN();
	CPushPayload payload;
	
	Json::Value test;
	Cplatform platform(all);
	Coptions options(false);
	Caudience audience(REGISTRATION_ID,ids);	
	Cnotification notification(alert,title,build_id,test);
	
	payload.setPlatform(&platform);
	payload.setAudience(&audience);
	payload.setNotification(&notification);
	payload.setOptions(&options);

	CcurlHandle *curlHandle  = CcurlModule::curlGetHandle();
	Json::FastWriter writer;
	std::string tmp3 = writer.write(payload.toJson());
	std::cout<<tmp3<<std::endl;
	
	std::string targetFileds = std::string(tmp3.c_str(),tmp3.length()-1);  //json对象转成字符串之后最后总有一个换行，-1删掉它
	curlHandle->setOption(CURLOPT_POSTFIELDS,targetFileds.c_str());

	CURLcode ret = curlHandle->perform();
	CcurlModule::curlRelessHandle(curlHandle);
	
	return ret;

	TRACE_OUT();
}




