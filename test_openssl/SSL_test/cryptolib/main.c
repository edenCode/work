#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cryptolib.h"

#define BUF_SIZE 1024 * 1024 *2
int main(int argc,const char * argv[])
{
    int i = 0;
    int ret = -1;
    char * str = "This is a example,This is a example,This is a example,This is a example,This is a example,This is a example,This is a example,This is a example,This is a example,This is a example";
     //char * str = "This is a example";
    char * chiper = (char *)malloc(BUF_SIZE);
    char * result = (char *)malloc(BUF_SIZE);

    ret = base64_encode(str,strlen(str)+ 1,chiper);
    printf("chiper size : %d \n",ret);
    printf("chiper      ：%s \n",chiper);


    ret = base64_decode(chiper,ret,result);
    printf("chiper size : %d \n",ret);
    printf("chiper      ：%s \n",result);

    memset(chiper,0,BUF_SIZE);
    memset(result,0,BUF_SIZE);
    
    ret = base_md5_encode(str,strlen(str)+1,chiper);
    base_hex_encode(chiper,strlen(chiper),result);
    printf("MD5 : %s \n",result);

    memset(chiper,0,BUF_SIZE);

    base64_encode(str,strlen(str)+1,chiper);
    ret = base_rsa_encode(chiper,strlen(chiper),result,RSA_PKCS1_PADDING);
    memset(chiper,0,BUF_SIZE);
    base64_encode(result,strlen(result),chiper);
    printf("chiper : %s \n chiper_len %ld \n",chiper,strlen(chiper));
    //printf("\n\n");
  
    memset(chiper,0,BUF_SIZE);
    ret = base_rsa_decode(result,strlen(result),chiper, RSA_PKCS1_PADDING);
    printf("Result : %s \n",chiper);
    memset(result,0,BUF_SIZE);
    base64_decode(chiper, strlen(chiper),result);
    printf("Reslut : %s \n",result);
 #if 0   
    memset(chiper,0,BUF_SIZE);
    memset(result,0,BUF_SIZE);
    ret = base_rsa_encode(str,strlen(str),chiper,RSA_NO_PADDING);
    printf("chiper : %s \n",chiper);
    ret = base_rsa_decode(chiper,strlen(chiper),result, RSA_NO_PADDING);
    printf("Result : %s \n",result);

    memset(chiper,0,BUF_SIZE);
    memset(result,0,BUF_SIZE);
    ret = base_rsa_encode(str,strlen(str),chiper,RSA_SSLV23_PADDING);
    printf("chiper : %s \n",chiper);
    ret = base_rsa_decode(chiper,strlen(chiper),result, RSA_SSLV23_PADDING);
    printf("Result : %s \n",result);
 #endif   
	return 0;
}