#include <iostream>
#include <string>

#include <stdio.h>
#include <poll.h>

extern "C"{
#include "cryptolib.h"
}
#include "sslClient.h"


void ShowCerts(SSL * ssl)
{
    int ret = -1;
    X509 *cert;
    char *line;
    X509_NAME * x509_name;
    char name[512];
    int len = 512;
    
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL)
    {
        printf("数字证书信息:\n");
        
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证  书: %s\n", line);
        free(line);
        
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n", line);
        free(line);

        printf("使用者: \n");
        x509_name = X509_get_subject_name(cert);
        ret = X509_NAME_get_text_by_NID(x509_name, NID_countryName, name, len);
        printf("Return : %d,NID_countryName : %s \n",ret,name);
        memset(name, 0,len);
        ret =X509_NAME_get_text_by_NID(x509_name, NID_organizationName, name, len);
        printf("Return : %d,NID_organizationName : %s \n",ret,name);
        memset(name, 0,len);

        ret =X509_NAME_get_text_by_NID(x509_name, NID_organizationalUnitName, name, len);
        printf("Return : %d,NID_organizationalUnitName : %s \n",ret,name);
        memset(name, 0,len);

        ret =X509_NAME_get_text_by_NID(x509_name,NID_commonName,name, len);
        printf("Return : %d,NID_commonName : %s \n",ret,name);
        memset(name, 0,len);

        printf("颁发者: \n");
        x509_name = X509_get_issuer_name(cert);
        ret = X509_NAME_get_text_by_NID(x509_name, NID_countryName, name, len);
        printf("Return : %d,NID_countryName : %s \n",ret,name);
        memset(name, 0,len);
        ret =X509_NAME_get_text_by_NID(x509_name, NID_organizationName, name, len);
        printf("Return : %d,NID_organizationName : %s \n",ret,name);
        memset(name, 0,len);

        ret =X509_NAME_get_text_by_NID(x509_name, NID_organizationalUnitName, name, len);
        printf("Return : %d,NID_organizationalUnitName : %s \n",ret,name);
        memset(name, 0,len);

        ret =X509_NAME_get_text_by_NID(x509_name,NID_commonName,name, len);
        printf("Return : %d,NID_commonName : %s \n",ret,name);
        memset(name, 0,len);

        EVP_PKEY * pkey = X509_get0_pubkey(cert);
        //i2d_RSAPublicKey();
        //i2d_RSAPublicKey_fp(stdin,pkey->pkey.rsa);
        //printf("ASN 1.0 : ");
        unsigned char * pp = NULL;
        
        ret = i2d_PublicKey(pkey,&pp);
        printf("Return %d ,Publickey : %s \n",ret,pp);

        char pukey[1024];
        base_hex_encode(pp,ret,pukey);
        printf("hex key : %s \n",pukey);
        
        X509_free(cert);
    }
    else
    {
        printf("无证书信息！\n");
    }
    
    if( (ret = SSL_get_verify_result(ssl)) != X509_V_OK)
    {
        printf("证书验证失败！,Return %d \n",ret);
    }

   
}

int SSLClient::Connect()
{
    int ret = -1;
    struct sockaddr_in addr;
    
    if(s->fd < 0)
    {
        cout << " sockfd error \n";
        return -1;
    }
    
    bzero(&addr,sizeof(addr));
     
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

        
    ret = connect(s->fd, (struct sockaddr *)&addr, sizeof( addr ));
    if(ret < 0)
    {
        perror("connect ");
        return -2;
    }

    
    // 非阻塞
    int flag = fcntl(s->fd, F_GETFL, 0);
    fcntl(s->fd, F_SETFL, flag | O_NONBLOCK);

    
    return ret;
}

int SSLClient::SslConnect()
{
    int tryCount   = 7;
    int ret = 0;
    int events = POLLIN | POLLOUT;

    if(!s->ctx)
    {
        printf(" SSL socket Not Init ctx\n");
        return -1;
    }

    ret = Connect();
    if(ret < 0)
    {
        printf(" Tcp connect failed \n");
        return -1;
    }
    
    if(!s->ssl && ret >= 0)
    {
        /* 基于 ctx 产生一个新的 SSL */
        s->ssl = SSL_new(s->ctx);
        if (s->ssl == NULL) {   
            ERR_print_errors_fp(stderr);

            
        }
    }      

    if( s->fd < 0 && !s->ssl)
    {
        printf(" SSL socket Not Init \n");
        return -1;
    }    
     
    SSL_set_fd(s->ssl,s->fd);

   

#if 0
    ret = SSL_connect(s->ssl);

    int err = SSL_get_error(s->ssl, ret);

    if(ret > 0)
    {
           
            ShowCerts(s->ssl);
            return 0;
    }
    else
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
#endif
    SSL_set_connect_state(s->ssl);
    
    do
    {
        ret = SSL_do_handshake(s->ssl);
        
        
        if(ret >= 0)
        {
           
            ShowCerts(s->ssl);
            return 0;
        }
        
        int err = SSL_get_error(s->ssl, ret);
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
        pfd.fd = s->fd;
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

    return -2;
}

