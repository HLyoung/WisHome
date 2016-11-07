

#ifndef ENCRYPT_H_
#define ENCRYPT_H_ 

#include <openssl/des.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class DES3{
public:
    static bool EncodeCBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen);
	static bool EncodeEBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen);
	static bool DecodeCBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen);
	static bool DecodeEBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen);
	static bool SetKey(const  char*key1,const  char *key2,const  char *key3,const char * iv);
	
private:
	static DES_key_schedule ks1,ks2,ks3;
	static DES_cblock _iv;
	static bool _isInit;
	
	
};
#endif