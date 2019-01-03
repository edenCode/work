#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/select.h>

extern "C"{
#include <openssl/ssl.h>
#include <openssl/err.h>
}
using namespace std;

typedef enum{
    SSL_CLIEN,
    SSL_SERVER
}SSLSocket_type;
class SSLSocket{
public:
    int id;
    int port;
    string ip;
    string host;
    int fd;
    SSL * ssl;
    SSL_CTX *ctx;
    int socket_type;
public:

    SSLSocket(int sock_type,int sfd):ssl(NULL),socket_type(sock_type),fd(sfd){

        init(sock_type);

        if(sock_type == SSL_CLIEN)
            cout << "Client socket \n";
        else
            cout << "Server socket \n";
    }

    SSLSocket(int sock_type,string ip,int port):ssl(NULL),socket_type(sock_type),ip(ip),port(port){
    
        fd = socket(AF_INET,SOCK_STREAM,0);
        if(fd < 0)
            cout << "Socket create failed \n";
        
        init(sock_type);
        if(sock_type == SSL_CLIEN)
            cout << "Client socket \n";
        else
            cout << "Server socket \n";
    }

    SSLSocket():ssl(NULL){

    }

    ~SSLSocket(){

   
        if(ctx)
            SSL_CTX_free(ctx);
        ctx = NULL;
        if(ssl)
            SSL_free(ssl);
        ssl = NULL;
    }


    int init(int sock_type)
    {

        /* SSL 库初始化 */
        SSL_library_init();
        /* 载入所有 SSL 算法 */
        OpenSSL_add_all_algorithms();
        /* 载入所有 SSL 错误消息 */
        SSL_load_error_strings();
        /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
        if(sock_type == SSL_SERVER )
            ctx = SSL_CTX_new(SSLv23_server_method());
        else if(sock_type == SSL_CLIEN)
            ctx = SSL_CTX_new(SSLv23_client_method());
        
        if(!ctx)
        {
            ERR_print_errors_fp(stdout);
            return -1;
        }

        printf("SSL Version : %s \n",SSL_version_str);
        return 0;
    }


    int recv(string & buffer)
    {
        int len = 0;
        int selectRet = 0;
        char tempBuf[BUFSIZ] = {0};
        //SSL * ssl = this->ssl;
        
        struct timeval tv;
        fd_set fs_read;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_ZERO(&fs_read);
        FD_SET(fd, &fs_read);

        if(!ssl)
            return -2;
        // 使用select接收数据，等待超时60时
        selectRet = select(fd + 1, &fs_read, NULL, NULL, &tv);
        if (selectRet < 0)
        {
            printf("recv: select false\n");
            return -1;
        }
        else if (0 == selectRet)
        {
            return 0;
        }
        
        len = SSL_read(ssl, tempBuf, BUFSIZ);
        
        // 如果select可以接收数据，但是又接收不到数据，认为是失败
        if (len <= 0)
        {
            printf("recv: select ok, buf SSL_read false\n");
            return -1;
        }
        
        buffer.append(tempBuf, len);
        printf("recv data, len = %d\n", len);
        
        printf("---------------------------------------------------\n");
        printf("%s\n", tempBuf);
        printf("---------------------------------------------------\n");
        
        return len;

    }
    
    int send(string & buffer)
    {
        int retLen       = 0;
        int sendLen      = 0;
        //SSL * ssl        = NULL; 
        int bufLen       = buffer.size();
        const char *pBuf = buffer.c_str();
        
        
        if(!ssl)
        {
            return -1;
        }
        
 
        /* 发消息给服务器 */
        
        while(bufLen > 0)
        {
            retLen = SSL_write(ssl, pBuf + sendLen, bufLen);
        
            if (retLen < 0)
            {
                // 发送被中断，或将被阻塞，或要求重发，就重新发送
                if (errno == EINTR  || errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    continue;
                }
                printf("send false！errno:%d, %s !\n", errno, strerror(errno));
                return -1;
            }
            else
            {
                sendLen += retLen;
                bufLen  -= retLen;
                
                printf("send ok, len = %d\n", retLen );
            }
        }
        
        return sendLen;

    }
    
    int close_s()
    {
        int ret = 0;
        if(!this->ctx)
            return -1;
        if(this->fd)
            close(this->fd);
        if(this->ssl)
            SSL_free(this->ssl);
        
        return ret;
    }

};

