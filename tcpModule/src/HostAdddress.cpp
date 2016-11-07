

#include "HostAddress.h"

CHostAddress::CHostAddress(const HOST_ADDRESS_POINTER host_address,const void* handle)
{
	memcpy(&host_address_,host_address,sizeof(HOST_ADDRESS));
	handle_ = handle;
}

CHostAddress::CHostAddress()
{
	memset(&host_address_,0,sizeof(HOST_ADDRESS));
	handle_ = NULL;
}

CHostAddress::~CHostAddress()
{
	
}

HOST_ADDRESS_POINTER CHostAddress::GetAddress()
{
	return &host_address_;
}

const void* CHostAddress::GetHandle()
{
	return handle_;
}

string CHostAddress::GetKey()
{
	string key_string = GetKey((const char *)host_address_.ip,host_address_.port);
	return key_string;
}

bool CHostAddress::IsValid()
{
	if(handle_ == 0 || host_address_.port == 0)
		return false;
	else	
		return true;
}

string CHostAddress::GetKey(const HOST_ADDRESS_POINTER host_address)
{
	string key_string = GetKey((const char*)host_address->ip,host_address->port);
	return key_string;
}

string CHostAddress::GetKey(const char *ip,const UINT32 port)
{
	stringstream key_stream;
	key_stream<<(const char *)ip<<":"<<port;
	
	string key_string;
	StreamToString(key_stream,key_string);
	RemoveComma(key_string);
	
	return key_string;
	
}