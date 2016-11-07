

#ifndef _PROTOCOL_PARSER_H_
#define _PROTOCOL_PARSER_H_

extern "C"{
//#include "Base64.h"
//#include "wis.h"
}
#include "IParseProtocolModule.h"
#include <string>
#include <string.h>
#include "macro.h"
#include "tools.h"
#include "libLog.h"
#include "XML/tinyxml.h"

class CProtocolParser
{
public:
	CProtocolParser();
	CProtocolParser(TParserCallBack &tParserCallBack);
	
	bool ParseSocketProtocol(const char *recvbuf,UINT32 recvlen);
	bool MakeProtocolForSocket(UINT32 command, const char* input, \
	UINT32 inputlen, char* output, UINT32 *outputlen);
	UINT32 getCheckSum(const char *recvbuf,UINT32 recvlen);
private:
	void ParsePkgData();
	bool CheckCheckSum(const char *recvbuf,UINT32 recvlen);

private:
	TParserCallBack m_tParserCallBack;
	CSmartBuffer sBuffer;
	
};

#endif