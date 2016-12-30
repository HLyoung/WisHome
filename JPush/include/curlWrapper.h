#ifndef CURLHELPER_H_ 
#define CURLHELPER_H_

#include "curl/curl.h"
#include <string>

class CcurlHandle{
public:
	CcurlHandle(CURL *handle);
	~CcurlHandle();
	CURLcode setOption(CURLoption option,const void *parameter){
		return curl_easy_setopt(handle_,option,parameter);
	}
	CURLcode perform(void);
	void curlAppendHeader(char *tag,char* value);
	void configHandleForJPush(void);
	void curlDeleteHander(char *tag);
	char *errorMessage();
	CURL *getHandle(){return handle_;}
	
private:
	CURL *handle_;
	CURLcode errorno_;
	struct curl_slist*  plist;
};




class CcurlModule{
public:
	static CURLcode curlModuleInit(long flag = CURL_GLOBAL_ALL){return curl_global_init(flag);}
	static void curlModuleReless(void){curl_global_cleanup();}
	
	static CcurlHandle* curlGetHandle(void);
	static void curlRelessHandle(CcurlHandle *handle){curl_easy_cleanup(handle->getHandle());}
	static char* curlGetVersion(void){return curl_version();}
};
#endif //CURLHELPER_H_

