
#ifndef _IPARSE_PROTOCOL_MODULE_H_
#define _IPARSE_PROTOCOL_MODULE_H_

#include "macro.h"

typedef void (*ParserCallback)(UINT32 wEvent,UINT32 wResultCode,UINT32 wDataLen,\
const char *pData,void *pOwner);

typedef struct _TParserCallBack
{
	void *pOwner;
	ParserCallback pParserCallback;
}TParserCallBack;

class ICParseProtocolModule
{
public:
	virtual HANDLE RegisteCallBack(TParserCallBack &tParserCallBack) = 0;
	virtual bool FreeCallBack(HANDLE hParser) = 0;
	virtual bool ParseSocketProtocol(HANDLE hParser,const char *recvbuf,\
	UINT32 recvlen) = 0;
	virtual bool MakeProtocolForSocket(HANDLE hParser,UINT32\
	 command,const char* input, UINT32 inputlen,char *output,UINT32* outputlen) = 0;
};

extern "C" ICParseProtocolModule* GetParseProtocolModuleInstance();


#endif