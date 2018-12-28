#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "crypto.h"

#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/pem.h"

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
    
	return 0;
}
