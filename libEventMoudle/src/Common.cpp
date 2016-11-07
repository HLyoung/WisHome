

#include "Common.h"

bool sLink::operator==(const sLink &target)
{
	if(fd == target.getFd() && m_RemoteAddr == target.getRemoAddr())
		return true;
	else
		return false;
}


sLink& sLink::operator=(const sLink &target)
{
	fd = target.getFd();
	m_RemoteAddr = target.getRemoAddr();
	return *this;
}