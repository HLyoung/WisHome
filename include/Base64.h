
#ifndef ___BASE64_H___
#define ___BASE64_H___

#include <string>

#include <vector>

class Base64
{
public:
	typedef unsigned char byte;
	typedef std::vector<byte> Bytes;

	size_t static calcOutLength( size_t inLen ) { return inLen ? ( (inLen-1)/3+1)*4 : 0 ; }

	bool static encode(const std::string& in, std::string& out);

	bool static encode(const byte* in, size_t inLen, std::string& strOut);
	bool static encode(const byte* in, size_t inLen, Bytes& bytes);
	
	bool static decode(const std::string& strIn, std::string& strOut );

	bool static decode(const byte* in,  size_t inLen, std::string& strOut ) ;
	bool static decode(const byte* in,  size_t inLen, Bytes& bytes);
	

};

#endif // ___BASE64_H___
