

#include "curlWrapper.h"
#include "UniteDataModule.h"
#include "Base64.h"
#include "tinyxml.h"
#include "tinystr.h"

CcurlHandle::CcurlHandle(CURL *handle){
	handle_ = handle;
	errorno_ = CURLE_OK;
	configHandleForJPush();
}

void CcurlHandle::configHandleForJPush()
{
	CXmlDocument DBconfigXml;
	CUniteDataModule::GetInstance()->GetConfigXml(DBconfigXml);

	CXmlElement *pDocElement = DBconfigXml.RootElement();
	CXmlElement *pDBAccessElement = pDocElement->FirstChildElement("JPush");
	if(pDBAccessElement == NULL ) return;
	
	std::string appkey = pDBAccessElement->Attribute("appKey");
	std::string masterSecret = pDBAccessElement->Attribute("masterSecret");
	std::string auth = appkey + ":" + masterSecret;
	
	//std::string encode_auth;
	//if(Base64::encode(auth,encode_auth))
	//{}
	curl_easy_setopt(handle_,CURLOPT_USERPWD,auth.c_str());
	
//	const char *s = encode_auth.c_str();
	//std::cout<<encode_auth<<std::endl;
	
	curl_easy_setopt(handle_,CURLOPT_URL,"https://api.jpush.cn/v3/push");
	curl_easy_setopt(handle_,CURLOPT_POST,1);
	
	curl_slist *plist = curl_slist_append(NULL,   
                "Content-Type:application/json");  
    curl_easy_setopt(handle_,CURLOPT_HTTPHEADER, plist); 
}
CcurlHandle::~CcurlHandle()
{
	curl_easy_cleanup(handle_);
}

CURLcode CcurlHandle::perform(void){
	errorno_ = curl_easy_perform(handle_);
	return errorno_;
}

char *CcurlHandle::errorMessage(){
	return (char *)curl_easy_strerror(errorno_);
}

void CcurlHandle::curlAppendHeader(char *tag,char *value)
{
	std::string header = tag;
	header += ":";
	header += value;
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers,header.c_str());
	curl_easy_setopt(handle_,CURLOPT_HTTPHEADER,headers);
}

void CcurlHandle::curlDeleteHander(char *tag)
{
	std::string header = tag;
	header += ":";
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers,header.c_str());
	curl_easy_setopt(handle_,CURLOPT_HTTPHEADER,headers);
}

CcurlHandle* CcurlModule::curlGetHandle(void){
	CURL *url = curl_easy_init();
	CcurlHandle* handle = new CcurlHandle(url);
	return handle;
}

