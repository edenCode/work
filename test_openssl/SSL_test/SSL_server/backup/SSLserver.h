#include <iostream>
#include <string>
#include <vector>

#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

typedef struct{
    int id;
    int fd;
    int port;
    string ip;
    SSL * ssl;
}SSLClient;
class SSLServer{
private:

    int fd;
    int status;
    
    SSL_CTX *ctx;
 

    int counts;
    int max_client;
    vector<SSLClient *> clients;

    
    
    int ssl_server_init();

    int set_fd(int sfd );
    
public:
    SSLServer(int port,string ip):port(port),ip(ip),max_client(5){
            status = ssl_server_init();
    }
    SSLServer(){}
    ~SSLServer(){

        if(ctx)
            SSL_CTX_free(ctx);
     }
    
    int bind_s(int sfd);
    SSLClient * accpet_s(SSLClient * client);

    int recv(SSLClient *,string & buffer);
    int send(SSLClient *,string & buffer);
    int close_s(SSLClient *);

    int load_certfile(string & public_cert,string & private_cert);
};