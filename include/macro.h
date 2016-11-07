


#ifndef _MACRO_H_
#define _MACRO_H_

#include "limits.h"
#include "stdlib.h"

#if !defined(ACE_SIZEOF_INT)
#	if (UINT_MAX == 65535U)
#		define ACE_SIZEOF_INT 2
#	elif (UINT_MAX == 4294967295U)
#		define ACE_SIZEOF_INT 4	
#	elif (UINT_MAX == 18446744073709551615U)
#		define ACE_SIZEOF_INT 8
#	else
#       error("unsupported int size,must be update for this platform!")
#	endif /*UINT_MAX*/
#endif /*!defined (ACE_SIZEOF_INT)*/

#if ACE_SIZEOF_INT==4
	typedef char INT8;
	typedef int INT32;
	typedef long INT64;
	typedef unsigned char UINT8;
	typedef short int INT16;
	typedef unsigned int UINT32;
	typedef unsigned long UINT64;
	typedef void * HANDLE;
#elif ACE_SIZEOF_INT==8
    typedef char INT8;
	typedef short int INT32;
	typedef int INT64;
	typedef unsigned char UINT8;
	typedef unsigned short int UINT32;
	typedef unsigned int UINT64;
	typedef void * HANDLE;
#else
#	 error("unsupported int size,must be update for this platform!")
#endif  //ACE_SIZEOF_INT



#ifndef StreamToString
#define StreamToString(Stream,String)\
	do\
	{\
		String = Stream.str();\
	}while(0)
#endif  //StreamToString

#ifndef RemoveComma
#define RemoveComma(str)\
	do\
	{\
		string out_string = "";\
		for(int index = 0;index < str.length();index ++)\
		{\
			char c = str[index];\
			if(c == ',')\
			{\
				continue;\
			}\
			out_string += c;\
		}\
		str = out_string; \
	}while(0)
#endif //RemoveComma

#define LOCAL_INSTANCE(type) static type _local_instance_##type
#define GetLocalInstancePointer(type) &_local_instance_##type

#ifndef SafeDelete
#define SafeDelete(ptr)\
	do\
	{\
		if((ptr) != NULL) \
		{\
			delete (ptr);\
			(ptr) = NULL; \
		}\
	}while(0)
#endif //SafeDelete

#ifndef SafeDeleteArray
#define SafeDeleteArray(ptr) \
	do\
	{\
		if((ptr) != NULL) \
		{\
			delete[] (ptr);\
			(ptr) = NULL;\
		}\
	}while(0)
#endif //SafeDeleteArray

#ifndef SafeFree
#define SafeFree(ptr)\
	do{\
		if((ptr) != NULL)\
		{\
			free(ptr);\
			(ptr) = NULL;\
		}\
	}while(0)
#endif //SafeFree

#endif