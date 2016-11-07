

#include "ProtocolParser.h"
#include <iostream>

CProtocolParser::CProtocolParser()
{
	
}
CProtocolParser::CProtocolParser(TParserCallBack &tParserCallBack)
{
	m_tParserCallBack.pOwner = tParserCallBack.pOwner;
	m_tParserCallBack.pParserCallback = tParserCallBack.pParserCallback;
}

bool CProtocolParser::ParseSocketProtocol(const char *recvbuf,UINT32 recvlen)
{
	TRACE_IN();


#ifdef DEBUG	
	char *strBuffer  = new char[recvlen*2+1];
	HexToStr((UINT8*)strBuffer,(const UINT8*)recvbuf,recvlen);
	LOG_INFO("receive data:datalen = %d\n	data = %s\n",recvlen,strBuffer);
	delete[] strBuffer;
#endif

	sBuffer.append(recvbuf,recvlen);
	
	while(sBuffer.length() >= 12)
	{
		int packetDataLen = *((int *)(sBuffer.data() + 8));
		if(packetDataLen <= sBuffer.length() - 12)
		{
			if(!CheckCheckSum(sBuffer.data(),packetDataLen + 12))
				LOG_ERROR("checksum error");
			else
			{
				UINT32 wEvent = *((UINT32 *)(sBuffer.data() + 4));
				m_tParserCallBack.pParserCallback(wEvent,0,packetDataLen,sBuffer.data() + 12,m_tParserCallBack.pOwner);
			}
			sBuffer.truncate(packetDataLen + 12);			
		}
		else
			break;
		
	}

	/*int levelLen = recvlen;
	int heardIndex = 0;
	while(levelLen >= 12 && *((int *)(recvbuf + heardIndex + 8)) <= levelLen - 12)
	{
		int packLen = 12 +  *((int *)(recvbuf + heardIndex + 8));
		if(!CheckCheckSum(recvbuf + heardIndex , packLen ))
		{
			LOG_ERROR("checksum uncorrect");
            heardIndex += packLen;
            levelLen = recvlen - heardIndex;
			continue;
		}
		
		UINT32 wEvent = *((UINT32*)(recvbuf + heardIndex + 4));
		m_tParserCallBack.pParserCallback(wEvent,0,packLen - 12,(char *)(recvbuf + heardIndex + 12),m_tParserCallBack.pOwner);

		heardIndex += packLen;
		levelLen = recvlen - heardIndex;
	}*/
	TRACE_OUT();
}

bool CProtocolParser::MakeProtocolForSocket(UINT32 command, const char* input,\
 UINT32 inputlen,char *output,UINT32* outputlen)
{
	TRACE_IN();
	
    if(NULL == input || NULL == output )
	{
		LOG_ERROR("parameter illegal");
		return false;
	}		
	
	*((UINT32* )(output+4)) = command;
	memcpy(output + 12,input,inputlen);

    *((int *)(output + 8)) = inputlen;
	
	UINT32 checksum = getCheckSum(output+4,inputlen + 8);
	*((UINT32*)output) = checksum;
	
	*outputlen = inputlen + 12;
  
 #ifdef DEBUG
    char *strBuffer  = new char[(*outputlen)*2+1];
	HexToStr((UINT8*)strBuffer,(const UINT8*)output,*outputlen);
	LOG_INFO("after make portocol data for socket: %s",strBuffer);  //log中大于256字节就崩溃
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
	UINT32 checksum = *((int *)recvbuf);
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
