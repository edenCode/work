#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include <errno.h>
#include <sys/select.h>
#include "openssl/bio.h"
#include "openssl/x509.h"

#include "ssl.h"

#define DEBUG
#ifdef DEBUG
    #define LOG_(fmt,args ...) printf("[ %s %s ]"fmt,__DATE__,__TIME__,##args)
#else
    #define LOG_(fmt,args)
#endif

static SSL_CTX *ctx;
//static SSL * ssl[10];
//static SSL * ssl;
static int clients = 0;
#define MAX_CLIENTS 10




void printf_x509(X509* x509)
{
    printf("\n");
    BIO *bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
    //PEM_write_bio_X509(bio_out, x509);//STD OUT the PEM
    X509_print(bio_out, x509);//STD OUT the details
    //X509_print_ex(bio_out, x509, XN_FLAG_COMPAT, X509_FLAG_COMPAT);//STD OUT the details
    BIO_free(bio_out);
}


int SSL_lib_init()
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

    LOG_("SSL Version : %s \n",SSL_version_str);

    return 0;
}

int SSL_lib_free()
{
    if(ctx)
        SSL_CTX_free(ctx);
   
     if(ssl)
        SSL_free(ssl);

    return 0;
}

int SSL_load_certfile(char * public_cert,char * private_cert)
{
    int ret = 0;

    if(!ctx)
        return -1;
    
    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
    if (SSL_CTX_use_certificate_file(ctx,public_cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(ctx,private_cert, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        return -1;
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        return -1;
    }

    LOG_("%d %s \n",__LINE__,__func__);
    return ret;

}

int SSL_init(SSL_Sesion * sesion,int fd)
{
    SSL * ssl;
    int ret = 0;
    if(!sesion)
        return -1;
    
    if(!ssl && !sesion->ssl)
    {
        SSL_delete(sesion);
        ssl = SSL_new(ctx);
    }
    SSL_set_fd(ssl,fd);

    sesion->ssl = ssl;
    sesion->fd  = fd;
    sesion->id  = clients ++ % MAX_CLIENTS; 
    LOG_("%d %s \n",__LINE__,__func__);
    return ret;
}


int SSL_delete(SSL_Sesion * sesion)
{

    if(!sesion && !sesion->ssl)
        return -1;
    
    if(sesion->ssl)
        SSL_free(sesion->ssl);
    return 0;
}

int SSL_accept_s(SSL_Sesion * sesion)
{
    int ret = -1;

    if(!sesion && !sesion->ssl)
        return -1;
    ret = SSL_accept(sesion->ssl);
    if(ret < 0)
    {
        printf(" SSL_accept faile,ret %d \n",ret);
        close(sesion->fd);
        return ret;
    }

    return ret;
}

int SSL_connect_s(SSL_Sesion * sesion)
{    
    int tryCount   = 7;
    int ret = 0;
    int events = POLLIN | POLLOUT;
    int sslFalseCount;
    
    if(!sesion && !sesion->ssl)
        return -1;
  #if 0  
    ret = SSL_connect(sesion->ssl);
    if(ret < 0)
    {
        printf(" SSL_connect faile,ret %d \n",ret);
        close(sesion->fd);
        return ret;
    }
  #else
   SSL_set_connect_state(sesion->ssl);
      
      do
      {
          ret = SSL_do_handshake(sesion->ssl);
          
          
          if(ret >= 0)
          {
             
              //ShowCerts(s->ssl);
              LOG_("%d %s \n",__LINE__,__func__);
              X509 * x = SSL_get_peer_certificate(sesion->ssl);
              if(x)
                printf_x509(x);
              return 0;
          }
          
          int err = SSL_get_error(sesion->ssl, ret);
          if (err == SSL_ERROR_WANT_WRITE)                // 操作没完成，需要在下一次写事件中继续
          {
              events |= POLLOUT;
              events &= ~POLLIN;
          } else if (err == SSL_ERROR_WANT_READ)          // 操作没完成，需要在下一次读事件中继续   
          {
              events |= POLLIN;
              events &= ~POLLOUT;
          }
          else
          {
              ERR_print_errors_fp(stderr);
  
              return -1;
          }
          
          struct pollfd pfd;
          pfd.fd = sesion->fd;
          pfd.events = events;
          
          int poolRet = poll(&pfd, 1, 5000);    
          
          if(poolRet < 0) 
          {
  
              return -1;
          }
          else if(poolRet == 0 || pfd.revents & POLLERR)
          {
              // poolRet==0表示超时，算一次失败，
              // 或者有返回，但是是发生错误了
              // 再给多一些机会，下次再尝试
  
              ++sslFalseCount;
           
              if(sslFalseCount > 6)
              {
     
                  return -2;
              }
              
              
              return -1;
          }
          
          // 如果本次成功，会继续进行下一步握手
          // 因为是5次握手，只循环七次
          
      }while(tryCount--);

  #endif

    return -1;

}


int SSL_send(SSL_Sesion * sesion,char * buffer,int len)
{
    int retLen       = 0;
    int sendLen      = 0;
    SSL * ssl        = sesion->ssl; 
    int bufLen       = len;
    const char *pBuf = buffer;
    
    
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
    
    LOG_("%d %s \n",__LINE__,__func__);
    return sendLen;

}

int SSL_recv(SSL_Sesion * sesion,char * buffer,int len)
{
    int selectRet;
    int ret = 0;

    int size = 0;
    struct timeval tv;
    fd_set fs_read;
    int fd = -1;
    SSL * ssl = sesion->ssl;
    
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

    size = SSL_read(ssl,buffer,len);
    LOG_("%d %s \n",__LINE__,__func__);
    return size;
}


