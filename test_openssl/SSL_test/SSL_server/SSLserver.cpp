#include <iostream>
#include <string>

#include "SSLserver.h"

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
    
    counts ++;
    if(counts >= 5)
        printf(" waring,over than max client.The max clients is %d \n",max_client);

    s->id = client->id;
    
    return client->id;
}

