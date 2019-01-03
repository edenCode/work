#include <iostream>
#include <string>
#include <vector>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "sslSocket.h"

using namespace std;


class SSLServer{
private:
    int status;    
    int counts;
    int max_client;

    vector<SSLSocket *> clients;
    SSLSocket * s;
   
    
public:
    SSLServer(string ip,int port):max_client(5){
            s = new SSLSocket(SSL_SERVER,ip,port);
    }
    SSLServer(int sfd){s = new SSLSocket(SSL_SERVER,sfd);}
    ~SSLServer(){
        if(s)
            delete s;
     }

    /*绑定服务器地址 开始监听*/
    int bind_s();
    
    /*等待客户端连接*/
    int accpet_s();

    int send(string & buffer)
    { 
        SSLSocket * client = find_client(s->id);    
        return client->send(buffer); 
    }
    
    int recv(string & buffer)
    {
         SSLSocket * client = find_client(s->id);    
        
        return client->recv(buffer);
    }
    int Close(){ 
        SSLSocket * client = find_client(s->id); 

        return client->close_s();
    }

    
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

    SSLSocket * find_client(int id)
    {
        vector<SSLSocket *>::iterator it = clients.begin();
        SSLSocket * s = NULL;
        for( ; it != clients.end(); it ++)
        {
            s =*it;
            
            if(s->id == id)
                break;

            
            cout <<" Ip   : " << s->ip << endl;
            cout <<" port : " << s->port << endl;
            cout <<" Fd   :" << s->fd;
            cout << endl;
           
        }

        return *it;
    }
};
