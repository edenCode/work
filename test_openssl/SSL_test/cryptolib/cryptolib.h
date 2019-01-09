
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "openssl/md5.h"
#include "openssl/sha.h"
#include "openssl/aes.h"
#include "openssl/rsa.h"

#include "openssl/x509.h"
#include "openssl/x509v3.h"

void printf_x509(X509* x509);

int base_hex_encode(void* str, int strlen, char *ascii);

int base_md5_encode(const char * in,int insize,unsigned char * out);

int base_sha256_encode(const char * in,int insize,unsigned char * out);


int base64_encode(const char * data,int size,char * out);
int base64_decode(const char * data,int size,char * out);

int base_rsa_encode(const unsigned char * in,int size,unsigned char * out,int padding);

int base_rsa_decode(const unsigned char * in,int size,unsigned char * out,int padding);




