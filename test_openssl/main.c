#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "crypto.h"

#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/md5.h"

#define BLOCK_SIZE (1024 * 2)
#define false  0
#define true   1

#if 0
int base_hex_encode(void* str, int strlen, char *ascii, int size)
{
    if(strlen == 0|| str== NULL || ascii==NULL)
    {
        if(NULL != ascii)
            ascii[0]=0x00;
        return 0;
    }

     char* p1 = ascii;//new  char[len*2+1];
    unsigned char* p2 = (unsigned char*)str;
    int i = 0, j = 0;
    char dict[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int blCuted = false;
    for( i = 0; i < strlen; ++i)
    {
        p1[j] = dict[ (p2[i] >> 4)];
        p1[j+1] = dict[p2[i] & 0x0F];
        j+=2;

        if(j > size){
            blCuted = true;
            break;
        }
    }
    if(blCuted)
        j-=2 ;
    p1[j]  =0x00;
    return j;
}

#else
int base_hex_encode(const char * str,int size,char * out)
{
    int ret = -1;

    if(!out)
        return -1;
    
    while(*str)
    {
        *out++ = *str ++ & 0xff;
        
    }
    return 0;
}
#endif

int base_md5(const char * in,int insize,char * out)
{
    int ret = -1;
    char * ptr = in;
    MD5_CTX ctx;
    int nums = 1,left = 0; 
    if(!in || !out)
    {
        return -1;
    }

    MD5_Init(&ctx);

    if(insize > BLOCK_SIZE)
    {
        nums = insize / BLOCK_SIZE;
        left = insize % BLOCK_SIZE;
    }

    if(nums == 1)
        MD5_Update(&ctx,ptr,insize);
    else if(nums > 2)
    {
        while(nums --)
        {
            MD5_Update(&ctx,ptr,BLOCK_SIZE);
            ptr += BLOCK_SIZE;
        }

        if(left)
            MD5_Update(&ctx,ptr,left);
    }

    ret = MD5_Final(out,&ctx);
    
    return ret;
}

int base64_encode(const char * data,int size,char * out)
{
    BIO * bio = NULL;
    BIO * b64 = NULL;
    BUF_MEM * ptr = NULL;
    int ret = -1;
    if(!data || !out)
    {
        return -1;
    }
    
    b64 = BIO_new(BIO_f_base64()); // filter
    bio = BIO_new(BIO_s_mem());

    bio = BIO_push(b64,bio);    
    BIO_write(bio,data,size);
    BIO_flush(bio);

    BIO_get_mem_ptr(bio,&ptr);
    
    memcpy(out,ptr->data,ptr->length);
    out[ptr->length] = '\0';
    ret = ptr->length;
    
    BIO_free_all(bio);

    return ret;
}

int base64_decode(const char * data,int size,char * out)
{
    BIO * bio = NULL;
    BIO * b64 = NULL;
    BUF_MEM * ptr = NULL;
    int ret = -11;
    if(!data || !out)
    {
        return -1;
    }
    
    b64 = BIO_new(BIO_f_base64()); // filter
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    bio = BIO_new_mem_buf(data,size);
    bio = BIO_push(b64,bio);
    
    ret = BIO_read(bio,out,size);

    BIO_free_all(bio);

    return ret;
}


int main(int argc,const char * argv[])
{
    int i = 0;
    int ret = -1;
    char * str = "This is a example";
    char * chiper = (char *)malloc(1024 * 1024 *2);
    char * result = (char *)malloc(1024*1024*2);

    ret = base64_encode(str,strlen(str)+ 1,chiper);
    printf("chiper size : %d \n",ret);
    printf("chiper      ：%s \n",chiper);


    ret = base64_decode(chiper,ret,result);
    printf("chiper size : %d \n",ret);
    printf("chiper      ：%s \n",result);

    ret = base_md5(str,strlen(str)+1,result);
    printf("MD5 : ");
    for(i = 0; i < strlen(result); i ++)
        printf("%x",result[i]);
    printf("\n");
    printf("chiper size : %d \n",ret);

//    unsigned char * p = MD5();
    //printf("MD5         ：%s \n",result);
    
	return 0;
}
