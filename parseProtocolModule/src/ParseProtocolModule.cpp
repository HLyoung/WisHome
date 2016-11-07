

#include "ParseProtocolModule.h"

CParseProtocolModule *CParseProtocolModule::m_instance = NULL;
CParseProtocolModule::CParseProtocolModule()
{
	
}

CParseProtocolModule::~CParseProtocolModule()
{
	
}

CParseProtocolModule *CParseProtocolModule::GetInstance()
{
	if(NULL == m_instance)
	{
		m_instance = new CParseProtocolModule();
	}
	return m_instance;
}

HANDLE CParseProtocolModule::RegisteCallBack(TParserCallBack &tParserCallBack)
{
	CProtocolParser *pProtocolParser = new CProtocolParser(tParserCallBack);
	return (HANDLE)pProtocolParser;
}

bool CParseProtocolModule::FreeCallBack(HANDLE hParser)
{
	CProtocolParser *pParser = (CProtocolParser *)hParser;
	SafeDelete(pParser);
	return true;
}

bool CParseProtocolModule::ParseSocketProtocol(HANDLE hParser,const char *\
recvbuf,UINT32 recvlen)
{   
	CProtocolParser *pProtocolParser = (CProtocolParser *)hParser;
	if(NULL == pProtocolParser)
	{
		return false;
	}
	return pProtocolParser->ParseSocketProtocol(recvbuf,recvlen);
}

bool CParseProtocolModule::MakeProtocolForSocket(HANDLE hParser,\
UINT32 command,const char *input,UINT32 inputlen,char *output,UINT32 *outputlen)
{
	
	CProtocolParser *pProtocolParser = (CProtocolParser *)hParser;
	if(NULL == pProtocolParser)
	{
		return false;
	}
	return pProtocolParser->MakeProtocolForSocket(command,input,inputlen,output,outputlen);
}

extern "C" ICParseProtocolModule* GetParseProtocolModuleInstance()
{
	return CParseProtocolModule::GetInstance();
}