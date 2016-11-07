

#include "Base64.h"

using namespace std;

static const char *g_pCodes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static const Base64::byte g_pMap[256] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
	 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};

bool Base64::encode(const std::string& in, std::string& out)
{
	return encode( reinterpret_cast<const byte*>(in.c_str()), in.length(), out);
}

bool Base64::encode(const byte *in, size_t inLen, Bytes& outBytes )
{
	size_t i, len2, leven;
	byte *p;

	if( in == NULL || inLen == 0 )
		return false;

	len2 = ((inLen + 2) / 3) << 2;

	leven = 3 * (inLen / 3);
	for(i = 0; i < leven; i += 3)
	{
		outBytes.push_back( g_pCodes[in[0] >> 2] );
		outBytes.push_back( g_pCodes[((in[0] & 3) << 4) + (in[1] >> 4)] );
		outBytes.push_back( g_pCodes[((in[1] & 0xf) << 2) + (in[2] >> 6)] );
		outBytes.push_back( g_pCodes[in[2] & 0x3f] );
		in += 3;
	}

	if (i < inLen)
	{
		byte a = in[0];
		byte b = ((i + 1) < inLen) ? in[1] : 0;
		byte c = 0;

		outBytes.push_back( g_pCodes[a >> 2] );
		outBytes.push_back( g_pCodes[((a & 3) << 4) + (b >> 4)] );
		outBytes.push_back( ((i + 1) < inLen ) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=' );
		outBytes.push_back( '=' );
	}
	outBytes.push_back( '\0' );
	return true;
}

bool Base64::encode(const byte *in, size_t inLen, string& strOut)
{
	size_t i, len2, leven;
	strOut = "";

	if( ! in ) return false;

	len2 = ((inLen + 2) / 3) << 2;
	leven = 3 * (inLen / 3);
	for(i = 0; i < leven; i += 3)
	{
		strOut += g_pCodes[in[0] >> 2];
		strOut += g_pCodes[((in[0] & 3) << 4) + (in[1] >> 4)];
		strOut += g_pCodes[((in[1] & 0xf) << 2) + (in[2] >> 6)];
		strOut += g_pCodes[in[2] & 0x3f];
		in += 3;
	}

	if (i < inLen)
	{
		byte a = in[0];
		byte b = ((i + 1) < inLen) ? in[1] : 0;
		byte c = 0;

		strOut += g_pCodes[a >> 2];
		strOut += g_pCodes[((a & 3) << 4) + (b >> 4)];
		strOut += ((i + 1) < inLen ) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		strOut += '=';
	}
	return true;
}

bool Base64::decode(const std::string& strIn, std::string& strOut )
{
	return decode( reinterpret_cast<const byte*>(strIn.c_str()), strIn.length(), strOut );
}

bool Base64::decode(const byte* in,  size_t inLen, std::string& strOut )
{
	size_t t, x, y, z;
	byte c;
	size_t g = 3;

	if( !in ) return false;

	for(x = y = z = t = 0; x < inLen; x++)
	{
		c = g_pMap[in[x]];
		if(c == 255) continue;
		if(c == 254) { c = 0; g--; }

		t = (t << 6) | c;

		if(++y == 4)
		{
			strOut += ( (byte)((t>>16)&255) ) ;
			if(g > 1) strOut += ( (byte)((t>>8)&255) );
			if(g > 2) strOut += ( (byte)(t&255) );
			y = t = 0;
		}
	}
	return true;
}


bool Base64::decode(const byte* in, size_t inLen, Bytes& bytes)
{
	size_t t, x, y, z;
	byte c;
	size_t g = 3;

	if( !in ) return false;

	for(x = y = z = t = 0; x < inLen; x++)
	{
		c = g_pMap[in[x]];
		if(c == 255) continue;
		if(c == 254) { c = 0; g--; }

		t = (t << 6) | c;

		if(++y == 4)
		{
			bytes.push_back( (byte)((t>>16)&255) ) ;
			if(g > 1) bytes.push_back( (byte)((t>>8)&255) );
			if(g > 2) bytes.push_back( (byte)(t&255) );
			y = t = 0;
		}
	}
	return true;
}

