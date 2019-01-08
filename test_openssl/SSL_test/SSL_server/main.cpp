#include <iostream>
#include <string>


#include "SSLserver.h"

#define PUBLIC_CA "./CAcert.pem"
#define PRIVATE_CA "./privkey.pem"
int main(int argc,char * argv[])
{
    int ret = -1;
    SSLServer * s;
    std::string buffer;
    std::string prikey(PRIVATE_CA);
    std::string pubkey(PUBLIC_CA);

    /*加载openssl*/
    s = new SSLServer("",8888);

    /*加载证书*/
    ret = s->load_certfile(pubkey,prikey);
    if(ret < 0)
    {
        printf("load_certfile failed \n");
        return -1;
    }

    /*绑定 服务器地址*/
    s->bind_s();

    while(1)
    {
       
        ret = s->accpet_s();

        while(ret >= 0)
        {
            ret = s->recv(buffer);
            std::cout<< "Recv : "<< buffer<< " recv len : " << ret << std::endl;
            sleep(1);
            ret = s->send(buffer);
            std::cout<< "Send : " << buffer<< " Send len : " << ret << std::endl;

            if(buffer == "quit")
                break;
        }

    }

    std::cout << " Server exit \n";
    return 0;
}
