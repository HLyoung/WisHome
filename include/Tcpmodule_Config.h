#ifndef ZXVNMS_TCPMODULE_CONFIG_H
#define ZXVNMS_TCPMODULE_CONFIG_H

#undef DLL_DEFINE

#ifdef WIN32
	#ifdef TCPMODULE_EXPORTS
		#define DLL_DEFINE __declspec (dllexport)
	#else
		#define DLL_DEFINE __declspec (dllimport)
	#endif
#else
	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef long  LONG;
	typedef unsigned char BYTE; 
	#define  CALLBACK
	#define DLL_DEFINE
#endif

#endif