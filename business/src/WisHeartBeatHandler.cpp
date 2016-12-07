



#include "WisHeartBeatHandler.h"

void  WisHeartBeatHandler::handleDeviceHeartBeat(BUS_ADDRESS_POINTER busAddress)
{
	TRACE_IN();
	unsigned int  current = (unsigned int )time(0);
 	GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_HEART_BEAT,(char *)&current,sizeof(unsigned int),TCP_SERVER_MODE);
	TRACE_OUT();
}

void WisHeartBeatHandler::handleUserHeartBeat(BUS_ADDRESS_POINTER busAddress)
{
	TRACE_IN();
	unsigned int current = (unsigned int)time(0);
	GetUniteDataModuleInstance()->SendData(busAddress,WIS_CMD_USER_HEART_BEAT,(char *)&current,sizeof(unsigned int ),TCP_SERVER_MODE);
	TRACE_OUT();
	
}