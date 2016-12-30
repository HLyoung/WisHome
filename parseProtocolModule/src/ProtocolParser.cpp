

#include "ProtocolParser.h"
#include <iostream>

CProtocolParser::CProtocolParser()
{
	sBuffer = new CSmartBuffer(0,1024);
}
CProtocolParser::CProtocolParser(TParserCallBack &tParserCallBack)
{
	m_tParserCallBack.pOwner = tParserCallBack.pOwner;
	m_tParserCallBack.pParserCallback = tParserCallBack.pParserCallback;
	sBuffer = new CSmartBuffer(0,1024);
}

CProtocolParser::~CProtocolParser()
{
	SafeDelete(sBuffer);
}


bool CProtocolParser::ParseSocketProtocol(const char *recvbuf,UINT32 recvlen)
{
	TRACE_IN();


#ifdef DEBUG	
	char *strBuffer  = new char[recvlen*2+1];
	HexToStr((UINT8*)strBuffer,(const UINT8*)recvbuf,recvlen);
	LOG_DEBUG("receive data:datalen = %d\n	data = %s\n",recvlen,strBuffer);
	delete[] strBuffer;
#endif

	sBuffer->append(recvbuf,recvlen);
	
	while(sBuffer->length() >= 12)
	{
		int packetDataLen = *((unsigned int *)(sBuffer->data() + 8));
		if(packetDataLen <= sBuffer->length() - 12)
		{
			if(!CheckCheckSum(sBuffer->data(),packetDataLen + 12)){
				sBuffer->empty();
				LOG_ERROR("CHECKSUM ERROR.");
				break;
				}
			else
			{
				UINT32 wEvent = *((UINT32 *)(sBuffer->data() + 4));
				m_tParserCallBack.pParserCallback(wEvent,0,packetDataLen,sBuffer->data() + 12,m_tParserCallBack.pOwner);
				sBuffer->truncate(packetDataLen + 12);			
			}
		}
		else
			break;
		
	}
	TRACE_OUT();
}

bool CProtocolParser::MakeProtocolForSocket(UINT32 command, const char* input,\
 UINT32 inputlen,char *output,UINT32* outputlen)
{
	TRACE_IN();	

	*((UINT32* )(output+4)) = command;
	if(input != NULL && inputlen != 0)
		memcpy(output + 12,input,inputlen);

    *((int *)(output + 8)) = inputlen;	
	UINT32 checksum = getCheckSum(output+4,inputlen + 8);
	*((UINT32*)output) = checksum;
	
	*outputlen = inputlen + 12;
  
 #ifdef DEBUG
    char *strBuffer  = new char[(*outputlen)*2+1];
	HexToStr((UINT8*)strBuffer,(const UINT8*)output,*outputlen);
	LOG_DEBUG("after make portocol data for socket: %s",strBuffer);  
	SafeDeleteArray(strBuffer);
 #endif 

	TRACE_OUT();
}

void CProtocolParser::ParsePkgData()
{
	
}

bool CProtocolParser::CheckCheckSum(const char *recvbuf,UINT32 recvlen)
{
	if(NULL == recvbuf)
	{
		LOG_INFO("recvbuf is NULL ");
		return false;
	}
	if(recvlen < 12)
	{
		LOG_INFO("buffer len is less then the min length");
		return false;
	}
	UINT32 checksum = *((unsigned int *)recvbuf);
	UINT32 tmpsum = 0;
	for(int i = 0;i<recvlen-4;i++)
	{
		tmpsum += (unsigned char)*(recvbuf + 4 + i);
	}
	if(tmpsum == checksum)
	{
		return true;
	}
	return false;
}

UINT32 CProtocolParser::getCheckSum(const char *recvbuf,UINT32 recvlen)
{
	UINT32 checksum  = 0;
	for(int i = 0;i<recvlen;i++)
	{
		checksum += (unsigned char )*(recvbuf + i);
	}
	
	return checksum;
}
