#include <iostream>
#include <string>

#include "SSLserver.h"


void ShowCerts(SSL * ssl)
{
    int ret = -1;
    X509 *cert;
    char *line;
    X509_NAME * x509_name;
    char name[512];
    int len = 512;

    /*获取对端的证书信息*/
    cert = SSL_get_peer_certificate(ssl);
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
            //print_c(sign->data,sign->length);
        
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
        char pubkey[1024];
        size_t len;
        EVP_PKEY_get_raw_public_key(pkey,(unsigned char *)pubkey,&len);
        printf("Return %ld ,Publickey : %s \n",len,pubkey);
        
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
    
    if( (ret = SSL_get_verify_result(ssl)) != X509_V_OK)
    {
        printf("证书验证失败！,Return %d \n",ret);
    }

   
}

int SSLServer::bind_s(){
    int ret = -1;
    struct sockaddr_in saddr;
 
    if(s->fd < 0)
        return -1;
    
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(s->port);
    saddr.sin_addr.s_addr = s->ip.size() <= 0 ? INADDR_ANY : inet_addr(s->ip.c_str());

    if(bind(s->fd,(struct sockaddr *)&saddr,sizeof(struct sockaddr)) < 0)
    {
        perror(" Bind :");
        return -1;
    }

    if(listen(s->fd,max_client) < 0)
    {
        perror(" Listen :");
        return -2;
    }

    return 0;
}


int SSLServer::accpet_s()
{
    int ret = -1;
    int client_fd = -1;
    struct sockaddr_in saddr;
    socklen_t len;
    SSLSocket * client = NULL;;

    if(s->fd > 0)
    {
        bzero(&saddr, sizeof(saddr));
        client_fd = accept(s->fd,(struct sockaddr *)&saddr,&len);
        if(client_fd < 0)
        {
            perror(" Accpet :");
            return -1;
        }

        client = new SSLSocket();//(counts,client_fd,inet_ntoa(saddr.sin_addr),ntohs(saddr.sin_port));

        /*保存每个连接客户端信息*/
        client->id = counts;
        client->port = ntohs(saddr.sin_port);
        client->ip   = inet_ntoa(saddr.sin_addr);
        client->fd   = client_fd;
        cout <<" Ip   : " << client->ip << endl;
        cout <<" port : " << client->port << endl;
        cout <<" Fd   :" << client->fd;
        cout << endl;
    }

    SSL * sl  = client->ssl;
    
    /* 基于 ctx 产生一个新的 SSL */
    if(!sl && s->ctx)
        sl = SSL_new(s->ctx);

    /* 将连接用户的 socket 加入到 SSL */
    if(client->fd < 0)
        return -1;
    SSL_set_fd(sl,client->fd);

    client->ssl = sl;
    clients.push_back(client);

    cout << " SSL_accept ... \n";
    /* 建立 SSL 连接 */
    if ((ret = SSL_accept(sl)) == -1)
    {
           // perror("accept");
	        cout << "SSL_accept failed,ret " << ret << endl;
            Close();
            return -1;
    }

    ShowCerts(sl);
    counts ++;
    if(counts >= 5)
        printf(" waring,over than max client.The max clients is %d \n",max_client);

    s->id = client->id;
    
    return client->id;
}

