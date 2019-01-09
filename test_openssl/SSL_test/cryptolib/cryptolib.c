#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "openssl/ossl_typ.h"
#include "openssl/err.h"

#include "cryptolib.h"

#define BLOCK_SIZE (1024 * 2)




void printf_x509(X509* x509)
{
    printf("\n");
    BIO *bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
    //PEM_write_bio_X509(bio_out, x509);//STD OUT the PEM
    X509_print(bio_out, x509);//STD OUT the details
    //X509_print_ex(bio_out, x509, XN_FLAG_COMPAT, X509_FLAG_COMPAT);//STD OUT the details
    BIO_free(bio_out);
}

#if 0
//x509 format
int  get_rsa_pubkey_from_cert(const char * file,RSA * rsa)
{
    X509 * cert;
    BIO * bio = BIO_new_file(file,"rb");
    RSA * rsa1;
    if(!bio || !rsa)
        return -1;
    
    cert = PEM_read_bio_X509(bio,&cert,NULL,NULL);

    EVP_PKEY * pkey = X509_get_pubkey(cert);
    if(!pkey)
        return -2;
    rsa1 = EVP_PKEY_get0_RSA(pkey);
    
    if(!rsa1)
        return -3;

    memcpy(rsa,rsa1,sizeof(struct rsa_st));

    BIO_free(bio);
    return 0;
}

/*!
*   Just PEM format.
*   
*/
int get_rsa_prikey_form_keyfile(const char * file,RSA * rsa)
{
    BIO * bio = BIO_new_file(file,"rb");
    RSA * rsa1;
    if(!bio)
        return -1;
    
    PEM_read_bio_RSAPrivateKey(bio,&rsa1,NULL,NULL);
    memcpy(rsa,rsa1,sizeof(struct rsa_st));
    BIO_free(bio);
    return 0;
}


#endif

int base_hex_encode(void* str, int strlen, char *ascii)
{
    if(strlen == 0|| str== NULL || ascii==NULL)
    {
        if(NULL != ascii)
            ascii[0]=0x00;
        return 0;
    }

    char* p1 = ascii;
    unsigned char* p2 = (unsigned char*)str;
    int i = 0, j = 0;
    int h,l;
    char dict[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    for( i = 0; i < strlen; ++i)
    {

            h = p2[i] / 16;
            l = p2[i] % 16;

            p1[j] = dict[h];
            j ++;
            p1[j] = dict[l];
            j ++;
      
    }
    p1[j - 2]  =0x00;
    return j;
}

int base_md5_encode(const char * in,int insize,unsigned char * out)
{
    int ret = -1;
    char * ptr = (char *)in;
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


int base_sha256_encode(const char * in,int insize,unsigned char * out)
{
    int ret = -1;
    char * ptr =(char *) in;
    SHA256_CTX ctx;
    int nums = 1,left = 0; 
    if(!in || !out)
    {
        return -1;
    }

    SHA256_Init(&ctx);

    if(insize > BLOCK_SIZE)
    {
        nums = insize / BLOCK_SIZE;
        left = insize % BLOCK_SIZE;
    }

    if(nums == 1)
        SHA256_Update(&ctx,ptr,insize);
    else if(nums > 2)
    {
        while(nums --)
        {
            SHA256_Update(&ctx,ptr,BLOCK_SIZE);
            ptr += BLOCK_SIZE;
        }

        if(left)
            SHA256_Update(&ctx,ptr,left);
    }

    ret = SHA256_Final(out,&ctx);
    
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

/*  flen must be less than RSA_size(rsa) - 11 for the PKCS #1 v1.5 based padding modes, 
 *   less than RSA_size(rsa) - 41 for RSA_PKCS1_OAEP_PADDING 
    and exactly RSA_size(rsa) for RSA_NO_PADDING . 
    The random number generator must be seeded prior to calling RSA_public_encrypt()
*/

/*
    openssl genrsa  -out privkey.pem 1024
    openssl req -new -x509 -key privkey.pem -out CAcert.pem -days 1095
    openssl rsa -in privkey.pem -pubout -out pubkey.pem
*/
#define PUBLIC_CERT "./pubkey.pem"
int base_rsa_encode(const unsigned char * in,int size,unsigned char * out,int padding)
{
    int ret = -1;
   // RSA * rsa = RSA_new();
   char chiper[256];

    X509 * cert;
    FILE * fp = fopen(PUBLIC_CERT,"rb");
    RSA * rsa;

 
    rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
   // get_rsa_pubkey_from_cert(PUBLIC_CERT,rsa);
    if(!rsa)
        return -3;

    int rsa_size = RSA_size(rsa);
    printf("in size %d,rsa size %d \n",size,rsa_size);

    switch (padding)
    {
        case RSA_NO_PADDING:
            if(size > rsa_size)
                size = rsa_size;
            break;
         case RSA_PKCS1_PADDING:
            if(size > (rsa_size - 11))
                size = rsa_size - 11;
            break;
    }

    ret = RSA_public_encrypt(size,in,out,rsa,padding);
    if(ret < 0)
    {
        printf("RSA_public_encrypt,return %d ,err code %ld \n",ret,ERR_get_error());
         printf(" %s \n",ERR_error_string(ERR_get_error(),NULL));
        return ret;
    }

    RSA_free(rsa);
    fclose(fp);
    //base64_encode(chiper,strlen(chiper),out);
    return ret;
}

#define PRIVATE_CERT "./privkey.pem"
/**
**  
*/
int base_rsa_decode(const unsigned char * in,int size,unsigned char * out,int padding)
{
    int ret = -1;
    //RSA * rsa = RSA_new();
    char chiper[256];
    
    FILE * fp = fopen(PRIVATE_CERT,"rb");
    RSA * rsa;
    
    
     rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    
     if(!rsa)
         return -3;

   // get_rsa_prikey_form_keyfile(PUBLIC_CERT,rsa);

    int rsa_size = RSA_size(rsa);
    printf("in size %d,rsa size %d \n",size,rsa_size);
    
    size = rsa_size;

    do{
        
    ret = RSA_private_decrypt(size,in,out,rsa,padding);
    if(ret < 0)
    {
        printf("RSA_private_decrypt,return %d ,errcode %ld\n",ret,ERR_get_error());
        printf(" %s \n",ERR_error_string(ERR_get_error(),NULL));
        return ret;
    }
   }while(ret < 0);
    RSA_free(rsa);
    fclose(fp);
    return ret;

}
