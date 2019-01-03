
int base_hex_encode(void* str, int strlen, char *ascii);
int base_md5_encode(const char * in,int insize,char * out);
int base_sha256_encode(const char * in,int insize,char * out);

int base64_encode(const char * data,int size,char * out);
int base64_decode(const char * data,int size,char * out);



