#include <iostream>
#include <string>

#include "sslSocket.h"


using namespace std;

class SSLClient{
private:
    string ip;
    int port;
    int sslFalseCount;
 
    SSLSocket * s;

public:
   SSLClient(int fd){
        s = new SSLSocket(SSL_CLIEN,fd);
   }

   SSLClient(string ip,int port):ip(ip),port(port){
	    s = new SSLSocket(SSL_CLIEN,ip,port);
   }

   ~SSLClient(){
        if(s)
            delete s;
   }
   
   int Connect();

   int SslConnect();
   int send(string & buffer){ return s->send(buffer); }
   int recv(string & buffer){ return s->recv(buffer); }
   int Close(){ return s->close_s();}
   // https 双向认证
   int load_certfile(string & public_cert,string & private_cert)
   {
        int ret = 0;
        
        /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
        if (SSL_CTX_use_certificate_file(s->ctx, public_cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stdout);
            return -1;
        }
        /* 载入用户私钥 */
        if (SSL_CTX_use_PrivateKey_file(s->ctx,private_cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stdout);
            return -1;
        }
        /* 检查用户私钥是否正确 */
        if (!SSL_CTX_check_private_key(s->ctx)) {
            ERR_print_errors_fp(stdout);
            return -1;
        }
        
        return ret;

    }
};

