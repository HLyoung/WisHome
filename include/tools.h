


#ifndef _TOOLS_H_
#define _TOOLS_H_
#include "macro.h"

//nlen表示转换为字符串之后的长度
static void StrToHex(UINT8 *pbDest,const UINT8 *pbSrc,INT32 nLen)
{
	INT8 h1,h2;
	UINT8 s1,s2;
	int i;
	
	for(i=0;i<nLen;i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i + 1];
		s1 = toupper(h1) - 0x30;
		if(s1 > 9)
			s1 -= 7;
		s2 = toupper(h2) - 0x30;
		if(s2 > 9)
			s2 -= 7;
		pbDest[i] = s1*16 + s2;
	}
}

//nLen表示转换之前的十六进制表示时的长度。
static void HexToStr(UINT8 *pbDest,const UINT8 *pbSrc,int nLen)
{
	char ddl,ddh;
	int i;
	
	for(i = 0;i<nLen; i++)
	{
		ddh = 48 + pbSrc[i]/16;
		ddl = 48 + pbSrc[i]%16;
		if(ddh > 57) ddh = ddh + 7;
		if(ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2 + 1] = ddl;
	}
	
	pbDest[nLen*2] = '\0';
	
}


#endif