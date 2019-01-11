#include "openssl/ssl.h"
#include "openssl/err.h"

#define PUBLIC_CERT "./cert.pem"
#define PRIVATE_CERT "./private.pem"

typedef struct SSL_sesion{
    int id;
    int fd;
    SSL * ssl;
}SSL_Sesion;

SSL_Sesion session;

int SSL_lib_init();
int SSL_lib_free();


int SSL_load_certfile(char * public_cert,char * private_cert);

int SSL_init(SSL_Sesion * sesion,int fd);
int SSL_delete(SSL_Sesion * sesion);

int SSL_accept_s(SSL_Sesion * sesion);

int SSL_send(SSL_Sesion * sesion,char * buffer,int len);
int SSL_recv(SSL_Sesion * sesion,char * buffer,int len);


int SSL_connect_s(SSL_Sesion * sesion);


