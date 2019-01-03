
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

#include "SSLServer.h"

int SSLServer::set_fd(int sfd )
{
    if(sfd)
        fd = sfd;
    else
        fd = socket(AF_INET,SOCK_STREAM,0);
    
    if(fd < 0)
        return -1;
    return 0;
}

int SSLServer::ssl_server_init()
{
        /* SSL 库初始化 */
    SSL_library_init();
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();
    /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_server_method());
    if(!ctx)
    {
        ERR_print_errors_fp(stdout);
        return -1;
    }

    printf("SSL Version : %s \n",SSL_version_str);
    return 0;
}


int SSLServer::load_certfile(string & public_cert,string & private_cert)
{
    int ret = 0;

    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
    if (SSL_CTX_use_certificate_file(ctx, public_cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(ctx,private_cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        return -1;
    }

    return ret;
}

int SSLServer::bind_s(int sfd){
    int ret = -1;
    struct sockaddr_in saddr;
 
    
    ret = set_fd(sfd);
    if(ret < 0)
        printf(" socket create failed \n");

    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = ip.size() <= 0 ? INADDR_ANY : inet_addr(ip.c_str());

    if(bind(fd,(struct sockaddr *)&saddr,sizeof(struct sockaddr)) < 0)
    {
        perror(" Bind :");
        return -1;
    }

    if(listen(fd,max_client) < 0)
    {
        perror(" Listen :");
        return -2;
    }

    return 0;
}


SSLClient * SSLServer::accpet_s(SSLClient * client)
{
    int client_fd = -1;
    struct sockaddr_in saddr;
    socklen_t len;
    //SSLClient * client = NULL;;

    if(client == NULL)
    {
        bzero(&saddr, sizeof(saddr));
        client_fd = accept(fd,(struct sockaddr *)&saddr,&len);
        if(client_fd < 0)
        {
            perror(" Accpet :");
            return NULL;
        }

        client = new SSLClient;
    
        client->id = counts;
        client->port = ntohs(saddr.sin_port);
        client->ip   = inet_ntoa(saddr.sin_addr);
        client->fd   = client_fd;
    }

    SSL * ssl  = client->ssl;
    
    /* 基于 ctx 产生一个新的 SSL */
    if(!ssl)
        ssl = SSL_new(ctx);

    /* 将连接用户的 socket 加入到 SSL */
    if(client->fd < 0)
        return NULL;
    SSL_set_fd(ssl,client->fd);

    client->ssl = ssl;
    clients.push_back(client);

    /* 建立 SSL 连接 */
    if (SSL_accept(ssl) == -1)
    {
            perror("accept");
            close(client->fd);
            return NULL;
    }
    
    counts ++;
    if(counts > 5)
        printf(" waring,over than max client.The max clients is %d \n",max_client);
    return client;
}

int SSLServer::send(SSLClient * s,string & buffer)
{
    int retLen       = 0;
    int sendLen      = 0;
    SSL * ssl        = NULL; 
    int bufLen       = buffer.size();
    const char *pBuf = buffer.c_str();
    

    if(NULL == s)
    {
        return -1;
    }
    
    ssl = s->ssl;

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

int SSLServer::recv(SSLClient * s,string & buffer)
{
    int len = 0;
    int selectRet = 0;
    char tempBuf[BUFSIZ] = {0};
    SSL * ssl = s->ssl;
    
    struct timeval tv;
    fd_set fs_read;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&fs_read);
    FD_SET(s->fd, &fs_read);

    // 使用select接收数据，等待超时60时
    selectRet = select(s->fd + 1, &fs_read, NULL, NULL, &tv);
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

int SSLServer::close_s(SSLClient * s)
{
    int ret = 0;
    if(!s)
        return -1;
    if(s->fd)
        close(s->fd);
    if(s->ssl)
        SSL_free(s->ssl);

    return ret;
}
