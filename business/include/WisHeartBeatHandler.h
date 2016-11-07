

#ifndef _HEART_BEAT_
#define _HEART_BEAT_

#include "wis.h"
#include "IUniteDataModule.h"
#include "libLog.h"

class WisHeartBeatHandler {
	public:    
		static void handleDeviceHeartBeat( BUS_ADDRESS &busAddress);    
		static void handleUserHeartBeat( BUS_ADDRESS &busAddress);
		};


#endif

