

#ifndef _HOST_ADDRESS_H_
#define _HOST_ADDRESS_H_

#include "transfer_structs.h"
#include "libLog.h"
#include "macro.h"
#include <sstream>
#include <string>


using namespace std;
class CHostAddress
{
public:
	CHostAddress(const HOST_ADDRESS_POINTER host_address,const void* handle);
	CHostAddress();
	virtual ~CHostAddress();

	
public:
	HOST_ADDRESS_POINTER GetAddress();
	const void* GetHandle();
	string GetKey();
	bool IsValid();
	static string GetKey(const HOST_ADDRESS_POINTER host_address);
	static string GetKey(const char *ip,const UINT32 port);
	
private:
	HOST_ADDRESS host_address_;
	const void* handle_;
};

#endif