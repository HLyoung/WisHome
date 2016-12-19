
#include <mutex>
#include "HostAddressMap.h"
#include "TCPService.h"
#include "TCPServiceManager.h"

using namespace std;
CHostAddressMap::CHostAddressMap()
{
}

CHostAddressMap::~CHostAddressMap()
{

}

LOCAL_INSTANCE(CHostAddressMap);

const void* CHostAddressMap::GetHostAddress(const std::string key)
{
	CHostAddressMap * instance = GetLocalInstancePointer(CHostAddressMap);	

	std::lock_guard<std::mutex> lck(instance->map_lock);	
	HostAddressMap::iterator pos = instance->m_mapHostAddress.find(key);
	if(pos != instance->m_mapHostAddress.end()){
		CHostAddress *pHostAddress = (CHostAddress*)pos->second;
		if(pHostAddress->IsValid())
			return pHostAddress->GetHandle();
		}
	 return NULL;
}

bool CHostAddressMap::SetHostAddress(HOST_ADDRESS_POINTER host_address,void* handle)
{
	CHostAddress *pAddress = new CHostAddress(host_address,handle);
	if(pAddress->IsValid()){
		CHostAddressMap *instance = GetLocalInstancePointer(CHostAddressMap);
		string key_string = pAddress->GetKey();
		instance->RemoveHostAddress(key_string);
		
		std::lock_guard<std::mutex> lck(instance->map_lock);
		instance->m_mapHostAddress.insert(pair<string,CHostAddress*>(key_string,pAddress));
		return true;
		}
	return false;
}

void CHostAddressMap::RemoveHostAddress(HOST_ADDRESS host_address)
{
	TRACE_IN();
	CHostAddressMap *instance = GetLocalInstancePointer(CHostAddressMap);
	string key_string = CHostAddress::GetKey((char*)host_address.ip,host_address.port);
	instance->RemoveHostAddress(key_string,true);
	TRACE_OUT();
}

void CHostAddressMap::RemoveHostAddress(const string key,bool bDeleteLink)
{

	TRACE_IN();
	CHostAddressMap * instance = GetLocalInstancePointer(CHostAddressMap);
	
	std::lock_guard<std::mutex> lck(instance->map_lock);
	HostAddressMap::iterator pos = instance->m_mapHostAddress.find(key);
	if(pos != instance->m_mapHostAddress.end())
	{
		CHostAddress *pHostAddress = (CHostAddress *)pos->second;
		const void* nSocketHandle = pHostAddress->GetHandle();
		if(bDeleteLink)
		{
			GetCommonTCPManager()->CloseConnection((void*)nSocketHandle);
		}	
		SafeDelete(pHostAddress);
		instance->m_mapHostAddress.erase(pos);
	}
	TRACE_OUT();
}





