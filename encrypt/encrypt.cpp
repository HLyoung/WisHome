
#include "encrypt.h"
#include "macro.h"


const char * key1 = "00000000";
const char * key2 = "00000000";
const char * key3 = "00000000";
DES_cblock ivsetup = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

DES_key_schedule DES3::ks1;
DES_key_schedule DES3::ks2;
DES_key_schedule DES3::ks3;
DES_cblock DES3::_iv;
bool DES3::_isInit = false;

bool DES3::EncodeCBC(const unsigned char * in,int inLen,unsigned char *out,int *outLen){
	int len = (inLen/8 + (inLen % 8 ? 1 : 0))*8;
	char ch = 8 - inLen % 8;
	//unsigned char tmp[len] = {0};
	unsigned char *tmp = (unsigned char* )(new char(len));
	memcpy(tmp,in,inLen);
	memset(tmp + inLen,ch,len - inLen);

	for(int i = 0; i< len;i += 8){
		if(_isInit)
			DES_ede3_cbc_encrypt(tmp + i,out + i,8,&ks1,&ks2,&ks3,&_iv,DES_ENCRYPT);	
		else{
			DES_cblock ivec;
			memcpy(ivec,ivsetup,sizeof(DES_cblock));
			DES_key_schedule gKs1,gKs2,gKs3;
			DES_set_key_unchecked((C_Block*)key1,&gKs1);
			DES_set_key_unchecked((C_Block*)key2,&gKs2);
			DES_set_key_unchecked((C_Block*)key3,&gKs3);
			DES_ede3_cbc_encrypt(tmp + i,out + i,8,&gKs1,&gKs2,&gKs3,&ivec,DES_ENCRYPT);
		}
	}
	if(NULL != outLen)
		*outLen = len;

	SafeDeleteArray(tmp);
	return true;
}


bool DES3::EncodeEBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen){
	int len = (inLen/8 + (inLen % 8 ? 1 : 0))*8;
	char ch = 8 - inLen % 8;
	unsigned char *tmp = (unsigned char *)(new char(len));
	memcpy(tmp,in,inLen);
	memset(tmp + inLen,ch,len-inLen);
	for(int i = 0;i < len;i += 8){
		if(_isInit)
			DES_ecb3_encrypt((C_Block *)(tmp + i),(C_Block *)(out + i), &ks1, &ks2, &ks3,DES_ENCRYPT);
		else{
			DES_key_schedule gKs1,gKs2,gKs3;
			DES_set_key_unchecked((C_Block*)key1,&gKs1);
			DES_set_key_unchecked((C_Block*)key2,&gKs2);
			DES_set_key_unchecked((C_Block*)key3,&gKs3);			
			DES_ecb3_encrypt((C_Block *)(tmp + i),(C_Block *)(out + i), &gKs1, &gKs2, &gKs3,DES_ENCRYPT);
		}
	}
	if(NULL != outLen)
		*outLen = len;

	SafeDeleteArray(tmp);
	return true;
}
bool DES3::DecodeEBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen){
	if(inLen%8 != 0)
		return false;
	
	for (int i = 0; i < inLen; i += 8) {
		if(_isInit)
			DES_ecb3_encrypt((C_Block *)(in + i), (C_Block *)(out + i), &ks1, &ks2, &ks3, DES_DECRYPT);
		else{
			DES_key_schedule gKs1,gKs2,gKs3;
			DES_set_key_unchecked((C_Block*)key1,&gKs1);
			DES_set_key_unchecked((C_Block*)key2,&gKs2);
			DES_set_key_unchecked((C_Block*)key3,&gKs3);
			DES_ecb3_encrypt((C_Block *)(in + i), (C_Block *)(out + i), &gKs1, &gKs2, &gKs3, DES_DECRYPT);		
		}
    }
	if(NULL != outLen)
		*outLen = inLen;
	return true;
}
bool DES3::DecodeCBC(const unsigned char *in,int inLen,unsigned char *out,int *outLen){
	if(inLen%8 != 0)
		return false;
	
	for (int i = 0; i < inLen; i += 8) {
		if(_isInit)
			DES_ede3_cbc_encrypt(in + i, out + i,8, &ks1, &ks2, &ks3,&_iv, DES_DECRYPT);
		else{
			DES_cblock ivec;
			memcpy(ivec,ivsetup,sizeof(DES_cblock));
			DES_key_schedule gKs1,gKs2,gKs3;
			DES_set_key_unchecked((C_Block*)key1,&gKs1);
			DES_set_key_unchecked((C_Block*)key2,&gKs2);
			DES_set_key_unchecked((C_Block*)key3,&gKs3);
			DES_ede3_cbc_encrypt(in + i, out + i, 8,&gKs1, &gKs2, &gKs3, &ivec,DES_DECRYPT);		
		}
    }
	if(NULL != outLen)
		*outLen = inLen;
	return true;
}
bool DES3::SetKey(const char  *key1,const char *key2,const char *key3,const char * iv){
	DES_set_key_unchecked((C_Block*)key1,&ks1);
	DES_set_key_unchecked((C_Block*)key2,&ks2);
	DES_set_key_unchecked((C_Block*)key3,&ks3);
	
	memcpy(_iv,iv,sizeof(_iv));
	_isInit = true;
	
}
	
/*int main(int argc,char* argv[])
{
   const char* test = "4567jhi";
	
	
	unsigned char out[100] = {0};
	unsigned char deOut[100] = {0};
	int outLen = 0;
	int deOutLen = 0;
	DES3::EncodeCBC((const unsigned char *)test,strlen(test),out,&outLen);
	for(int i = 0;i<outLen;i++)
		printf("%c",out[i]);
	printf("\n");
	std::cout<<outLen<<std::endl;
	
	DES3::DecodeCBC((const unsigned char *)out,outLen,deOut,&deOutLen);
	for(int i = 0;i<deOutLen;i++)
		printf("%c",deOut[i]);
	printf("\n");
	std::cout<<deOutLen<<std::endl;
	
	unsigned char out2[100] = {0};
	unsigned char deOut2[100] = {0};
	
	int outLen2 = 0;
	int deOutLen2 = 0;
	DES3::EncodeEBC((const unsigned char*)test,strlen(test),out2,&outLen2);
	for(int i = 0;i<outLen2 ;i++)
		printf("%c",out2[i]);
	printf("\n");
	std::cout<<outLen2<<std::endl;
	
	DES3::DecodeEBC((const unsigned char*)out2,outLen2,deOut2,&deOutLen2);
	for(int i = 0;i<deOutLen2;i++)
		printf("%c",deOut[i]);
	printf("\n");
	std::cout<<deOutLen<<std::endl;
	
	return 0;
	
}*/


