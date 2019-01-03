#include <iostream>
#include <string>

#include "sslClient.h"


int main(int argc,char * argv[])
{
    int ret = -1;
    SSLClient * s;
    //SSLSocket * c = NULL;
    std::string buffer("dffffffffffffffff");
    std::string rbuffer;

    /*加载openssl*/
    s = new SSLClient("127.0.0.1",8888);

    /*加载证书 https 双向认证时需要加载*/
    //s->load_certfile(pubkey,prikey);
  
    ret = s->SslConnect();
    
    if(ret < 0)
    {
        printf(" SslConnect failed \n");
        return -1;
    }
    
    while(1)
    {
       ret = s->send(buffer);
      
       std::cout<< "Send : " << buffer << " Send len : " << ret << std::endl;
       sleep(1);
       ret = s->recv(rbuffer);
       std::cout<< "Recv : "<< rbuffer << " recv len : " << ret << std::endl;

       if(buffer == "quit")
          break;
   }

  

    std::cout << " Server exit \n";
    return 0;
}
