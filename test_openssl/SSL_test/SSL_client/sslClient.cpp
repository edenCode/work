#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>


#include "cryptolib.h"

#include "sslClient.h"
#include "openssl/asn1.h"
#include "openssl/asn1t.h"

typedef enum{
    X509_V1 = 0,
    X509_V2,
    X509_V3
}X509_VERSION;

int ShowVersion(int version)
{
    switch (version)
        {
            case X509_V1:
                printf("X509_V1 \n");
                break;
            case X509_V2: 
                break;
            case X509_V3:
                break;
        }
}

int print_char(unsigned char * p,int size)
{
    int i = 0;
    unsigned long ch;
    printf(" size  : %d  \n",size);
    for(i = 0; i < size; i ++)
    {
       // printf(" %d",*(p + i));
       //UTF8_getc();
    }
    printf("\n");
    return 0;
}
int print_c(unsigned char * p,int size)
{
    int i = 0;
    printf(" size  : %d  \n",size);
    for(i = 0; i < size; i ++)
    {
        printf(" %c",*(p + i));
    }
    printf("\n");
    return 0;
}

void ShowCerts(SSL * ssl)
{
    int ret = -1;
    X509 *cert;
    char *line;
    X509_NAME * x509_name;
    char name[512];
    int len = 512;
    
    cert = SSL_get_peer_certificate(ssl);
  #if 0
  if(cert)
    parse_x509(cert);
  #else
    if (cert != NULL)
    {
        long version = X509_get_version(cert);
        printf("数字证书版本 ： %ld \n",version);

        printf("数字证书信息:\n");
        
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证  书: %s\n", line);
        free(line);
        
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n", line);
        free(line);

        unsigned char * info = NULL;
        printf(" parse serialnum  ... \n");
        ASN1_INTEGER * serial =  X509_get_serialNumber(cert);
        //uint64_t  ser;
       // ASN1_INTEGER_get_uint64(&ser,serial);
        //ASN1_STRING_to_UTF8(&info,serial);
        //long ser = ASN1_INTEGER_get(serial);
        //printf("SerialNum : %s \n",info);
        if(serial)
        {
  
            BIGNUM * bn =  ASN1_INTEGER_to_BN((const ASN1_INTEGER *)serial,NULL);
            info = (unsigned char *)BN_bn2hex(bn);
            printf("serial Num : %s \n",info);
            
            OPENSSL_free(info);
            info = NULL;
        }

        ASN1_BIT_STRING * sign;
        X509_get0_signature(((const ASN1_BIT_STRING **)&sign),NULL,cert);
        //ASN1_STRING_to_UTF8(&info,(const ASN1_STRING *)sign);
        if(sign)
            print_c(sign->data,sign->length);
        
        printf("Signature : %s \n",sign->data);

     
        
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
        
        ret =X509_NAME_get_text_by_NID(x509_name,NID_pkcs9_emailAddress,name, len);
        printf("Return : %d,NID_pkcs9_emailAddress : %s \n",ret,name);
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

        ret =X509_NAME_get_text_by_NID(x509_name,NID_pkcs9_emailAddress,name, len);
        printf("Return : %d,NID_pkcs9_emailAddress : %s \n",ret,name);
        memset(name, 0,len);
#if 0
        const STACK_OF(X509_EXTENSION) * stack = X509_get0_extensions(cert);
        if(version == 2)
        {
            for(int i = 0; i < X509v3_get_ext_count(stack); i ++)
            {
                unsigned char * pp = NULL;
                X509_EXTENSION * ext = X509v3_get_ext(stack,i);
                ASN1_OBJECT * obj = X509_EXTENSION_get_object(ext);
                
                i2d_ASN1_OBJECT(obj,&pp);
                printf(" PP : %s \n",pp);
                
            }
        }
#endif
        struct tm tm;
        ASN1_TIME * time = X509_get_notBefore(cert);
        ASN1_TIME_to_tm(time,&tm);
        printf("Not Before : %s \n",asctime(&tm));
 
        time = X509_get_notAfter(cert);
        ASN1_TIME_to_tm(time,&tm);
        printf("Not After : %s \n",asctime(&tm));

        #if 1
        EVP_PKEY * pkey = X509_get_pubkey(cert);
        
        //unsigned char * pp = NULL;
        //ret = i2d_PublicKey(pkey,&pp); // ASN 1.0 =====> ASCII
        unsigned char pubkey[1024];
        size_t len;
        EVP_PKEY_get_raw_public_key(pkey,(unsigned char *)pubkey,&len);
        printf("Return %d ,Publickey : %s \n",len,pubkey);
        
        #else
        unsigned char * pukey;
        ASN1_BIT_STRING * str =  X509_get0_pubkey_bitstr(cert);
        i2d_ASN1_BIT_STRING(str,&pukey);
        printf("pubkey : \n  %s \n",pukey);
        #endif
        X509_free(cert);
    }
    else
    {
        printf("无证书信息！\n");
    }
#endif
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

        printf("SSL version : %s  \n",SSL_get_version(s->ssl));
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

