

#include "ISocketOwner.h"
#include "Common.h"
#include <string.h>
#include <stdlib.h>

CADDRINFO::CADDRINFO():m_nPort(-1)
{
     memset(m_pszIP,0,IP_LEN);
}

CADDRINFO::CADDRINFO(const CADDRINFO &target)
{
	m_nPort = target.m_nPort;
	memcpy(m_pszIP,target.GetIP(),strlen(target.GetIP())+1);
}

CADDRINFO::CADDRINFO(const char *pszIP,int nPort):m_nPort(nPort)
{
	memcpy(m_pszIP,pszIP,strlen(pszIP)+1);
}

char *CADDRINFO::GetIP() const
{   
 	return (char *)m_pszIP;
}

int CADDRINFO::GetPort() const
{
	return m_nPort;
}

int CADDRINFO::SetIPAddr(const char *ipTemp)
{
	if(memcpy(m_pszIP,ipTemp,strlen(ipTemp)+1))
		return 0;
	else
		return -1;		
}

int CADDRINFO::SetPortAddr(int port)
{
	this->m_nPort = port;
	return 0;
}

CADDRINFO & CADDRINFO::operator = (const CADDRINFO &target)
{
	m_nPort = target.GetPort();
	memcpy(m_pszIP,target.GetIP(),strlen(target.GetIP()) + 1);
	return *this;
}

int CADDRINFO::operator == (const CADDRINFO &target) const
{
	if(!memcmp(m_pszIP,target.GetIP(),strlen(target.GetIP()) + 1) && m_nPort==target.GetPort())
		return 1;
	else
		return 0;
}





	