


#ifndef _TRANSFER_STRUCTS_H_
#define _TRANSFER_STRUCTS_H_

#include "macro.h"
#include <string.h>
#include <string>

enum {TCP_SERVER_MODE = 1,TCP_CLIENT_MODE = 2};
enum {TCP_CONNECT_SHORT = 0,TCP_CONNECT_LONG = 1};
enum {RES_ = 0,QUERY_ = 1};
enum {WAY_ADDRESS = 0,WAY_TCPSERVICE = 1};
enum {HEARTBEAT_TIME = 5};    //heart beat time set to 5


typedef enum
{
	BUS_ADDRESS_TYPE_UDP = (UINT32) 1,
	BUS_ADDRESS_TYPE_TCP = 2,
	BUS_ADDRESS_TYPE_COM = 3,
	BUS_ADDRESS_TYPE_ShortMessage = 4,
	BUS_ADDRESS_TYPE_Handle = 5	
}BUS_ADDRESS_TYPE;

enum
{ 
	MESSAGE_Sent = (UINT32) 0x10,		//发送操作完毕事件
	MESSAGE_Receive = (UINT32) 0x11,	//接收操作完毕事件
	MESSAGE_Connect = (UINT32) 0x12,
	MESSAGE_Disconnect = (UINT32) 0x13,
	MESSAGE_Close = (UINT32) 0x14, //客户端被动关闭
	MESSAGE_Multicast = (UINT32) 0x15 //组播回复
};

enum {HOST_NAME_LENGTH = 64};

typedef struct tagHost_ADDRESS
{
	UINT32 size;
	UINT8 ip[ HOST_NAME_LENGTH ];
	UINT32 port;
	bool operator == (const tagHost_ADDRESS &tHostAddress)
	{
		if(tHostAddress.port == port && 0 == memcmp(ip,tHostAddress.ip,HOST_NAME_LENGTH))
			return true;
		else
			return false;
	}
	tagHost_ADDRESS &operator=(const tagHost_ADDRESS &tHostAddress){
		size = tHostAddress.size;
		memcpy(ip,tHostAddress.ip,HOST_NAME_LENGTH);
		port = tHostAddress.port;
		return *this;
	}
}HOST_ADDRESS,*HOST_ADDRESS_POINTER;

typedef char SIM_ID[256];
typedef struct tagBUS_ADDRESS
{
	UINT32 size;
	UINT32 bus_address_type;
	UINT32 model_type;
	HOST_ADDRESS host_address;
	
	bool operator == (const tagBUS_ADDRESS &tBusAddress){
		if(bus_address_type == tBusAddress.bus_address_type && model_type == tBusAddress.model_type &&host_address == tBusAddress.host_address)
			return true;
		else
			return false;
		}
}BUS_ADDRESS,*BUS_ADDRESS_POINTER;

typedef struct tagDEVICE_INFO
{
	BUS_ADDRESS_POINTER bus_address;
	bool is_logined;
	char login_type;
	std::string uuid;
}DEVICE_INFO,*pDEVICE_INFO;

typedef struct tagSMART_BUFFER
{
	UINT32 size;
	const char *data;
}SMART_BUFFER,*SMART_BUFFER_POINTER;

typedef struct tagRECEIVE_DATA
{
	BUS_ADDRESS_POINTER from;
	SMART_BUFFER data;
}RECEIVE_DATA,*RECEIVE_DATA_POINTER;

typedef HOST_ADDRESS SEND_ARGUMENT, *SEND_ARGUMENT_POINTER;
typedef struct tagSEND_RESULT
{
	UINT32 size;
	SEND_ARGUMENT argument;
	UINT32 error_code;
	SMART_BUFFER data;
} SEND_RESULT, *SEND_RESULT_POINTER;

typedef struct tagCUSTOM_DATA
{
	UINT32 size;
	UINT8 data[ 0 ];
} CUSTOM_DATA, *CUSTOM_DATA_POINTER;

typedef void (*TRANSFER_MESSAGE_CALLBACK)(void*,      //owner
										  UINT32,	   //message
										  UINT32,	   //message size
										  void *);      //message data
typedef TRANSFER_MESSAGE_CALLBACK BUS_SERVICE_MESSAGE_CALLBACK,MESSAGE_CALLBACK;
typedef struct tagINITIAL_ARGUMENTS
{
	HANDLE owner;
	MESSAGE_CALLBACK net_message_callback;
} INITIAL_ARGUMENTS, *INITIAL_ARGUMENTS_POINTER;



#endif 
