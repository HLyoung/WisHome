

#ifndef _PARSEPROTOCOL_MODULE_H_
#define _PARSEPROTOCOL_MODULE_H_

#include "macro.h"
#include "IParseProtocolModule.h"
#include "ProtocolParser.h"

class CParseProtocolModule : public ICParseProtocolModule
{
public:
	static CParseProtocolModule* GetInstance();
	virtual HANDLE RegisteCallBack(TParserCallBack &tParserCallBack);
	virtual bool FreeCallBack(HANDLE hParser);
	virtual bool ParseSocketProtocol(HANDLE hParser,const char *recvbuf,UINT32 recvlen);
	virtual bool MakeProtocolForSocket(HANDLE hParser,UINT32 command,const char *input,\
		UINT32 inputlen,char *output ,UINT32 *outputlen);
private:
	CParseProtocolModule(void);
	virtual ~CParseProtocolModule();
	
	
private:
	static CParseProtocolModule* m_instance;
	
};

#endif