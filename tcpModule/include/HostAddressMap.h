


#ifndef _HOST_ADDRESS_MAP_H_
#define _HOST_ADDRESS_MAP_H_

#include <mutex>
#include "HostAddress.h"
#include "libLog.h"
#include "ICommonTCPManager.h"
#include <map>

using namespace std;

class CHostAddressMap
{
public:
	CHostAddressMap();
	virtual ~CHostAddressMap();
public:
	static const void* GetHostAddress(const string key);
	static bool SetHostAddress(HOST_ADDRESS_POINTER host_address,void* handle);
	static void RemoveHostAddress(const string key,bool bDeleteLink = true);
	static void RemoveHostAddress(HOST_ADDRESS host_address);
	
private:
	std::mutex map_lock;
	typedef std::map<string,CHostAddress *> HostAddressMap;
	std::map<string,CHostAddress *> m_mapHostAddress;
};


#endif