#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include "mutex.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "udp_server.h"
#include "debug.h"
#include "version.h"
#include "logex.h"
#include "innmsgdef.h"
#include "innsock.h"
#include "sysdef.h"
#include "socket.h"

#include "tcp_sdk.h" //qsq


#ifdef NETCOMM_DEBUG_

#define DBG( fmt, args... ) printf( fmt, ## args )
#define DBG_HERE 			printf( "File: %s, Func: %s, Line: %d \n", __FILE__, __func__, __LINE__ )
#define DBG_FUNC_LINE( fmt, args... ) printf( "%s[%d]"fmt, __func__, __LINE__, ## args )

#else

#define DBG( fmt, args... )
#define DBG_HERE
#define DBG_FUNC_LINE( fmt, args... )

#endif
#define ERR( fmt, args... ) printf( "ERR:%s[%d]"fmt, __func__, __LINE__, ## args )


static bool bRun = true;

static void signal_handler(int signum)
{
	char log[256] = "";
	DBG_FUNC_LINE("######## NetComm signal_handler, signum is %d. ########\n", signum);
	snprintf(log, sizeof(log), "######## signal_handler, signum is %d. ########", signum);
	SEND_LOG(1, mdNetComm, llWarn, log);
	
	switch (signum) 
	{
		case SIGCHLD:
			DBG_FUNC_LINE("receive a SIGCHLD signal.\n");
			break;
		case SIGABRT:
		case SIGKILL:
			bRun = false;
			break;
		case SIGINT:
		case SIGTERM:
			bRun = false;
			break;
		case SIGPIPE:
			DBG_FUNC_LINE("receive a SIGPIPE signal.\n");
			break;
		default:
			break;
	}
}

/**
 * @fn		static void set_signal_handler(int signum, void func(int))
 *
 * @brief	å¯¹ç‰¹å®šçš„ä¿¡å·è®¾å®šå¤„ç†å‡½æ•°
 *
 * @param	[in] signum		ä¿¡å·å·
 * @param	[in] void func(int)	ä¿¡å·å¤„ç†å‡½æ•°
 *
 */
static void SetSignalHandler(void)
{
	struct sigaction sigAction;
	sigAction.sa_flags = 0;
	sigemptyset(&sigAction.sa_mask);
	sigaddset(&sigAction.sa_mask, SIGINT);
	sigAction.sa_handler = signal_handler;

	sigaction(SIGCHLD, &sigAction, NULL);
	sigaction(SIGABRT, &sigAction, NULL);
	sigaction(SIGTERM, &sigAction, NULL);
	sigaction(SIGKILL, &sigAction, NULL);
	sigaction(SIGINT, &sigAction, NULL);
	sigaction(SIGPIPE, &sigAction, NULL);
}

TcpClient *tcp_client = NULL;
TcpServer *tcp_server = NULL;
UdpServer *udp_server = NULL;
TcpSdk *tcp_sdk = NULL;	//add by qsq at 20160128

/*
	åˆ›å»ºçº¿ç¨‹å·¥ä½œçŽ¯å¢ƒ
	åˆå§‹åŒ–å„é¡¹èµ„æº
*/
static int CreateThreadObjects()
{
	tcp_client      = new TcpClient();
	tcp_sdk			= new TcpSdk();
	tcp_server      = new TcpServer(tcp_client,tcp_sdk);

	//udp_server      = new UdpServer();

	return 0;
}

static int StartThreads()
{
	tcp_client->Start();
	tcp_server->Start();
	tcp_sdk->Start();  //qsq

	//udp_server->Start();

	return 0;
}

static bool MainThreadLoop()
{
	bool enable_reboot = false;
	InnMsgHead imh;
	int flag = 0;
	int ret = 0;
	unsigned int count = 0;

	imh.cmd = INNMSG_HEART;
	imh.bodylen = sizeof(int);
	flag = mdNetComm;

	while (bRun) 
	{
		if(count < 3)
		{
	        /* send heart beat to daemon */
	        ret = send_message(module_name_[mdDaemon - 1], &imh, &flag, NULL, NULL);
			if(ret != 0)
			{
				count++;
				usleep(100000);
				ERR("Netcomm send INNMSG_HEART failed, count = %d.\n", count);
				continue;
			}
		}
		count = 0;
		sleep(5);
		
	}

	return enable_reboot;
}

static int TerminateThreads()
{
	DBG_FUNC_LINE("TerminateThreads start.\n");
	//udp_server->Terminate();
	tcp_server->Terminate();
	tcp_client->Terminate();
	tcp_sdk->Terminate();  //qsq

	//udp_server->Join();
	tcp_server->Join();
	tcp_client->Join();
	tcp_sdk->Join();  //qsq

	DBG_FUNC_LINE("TerminateThreads over.\n");
	return 0;
}

static int ReleaseResource()
{
	DBG_FUNC_LINE("ReleaseResource start.\n");
	if (udp_server) 
		delete udp_server;
	if (tcp_server) 
		delete tcp_server;
	if (tcp_client) 
		delete tcp_client;
	if (tcp_sdk)	//qsq
		delete tcp_sdk;
	
	DBG_FUNC_LINE("ReleaseResource over.\n");
	return 0;
}
typedef  struct Data_YUV{
	int w;
	int h;
	int size;
	int type;
	unsigned char * buffer;
}Data_YUV;

typedef struct TagInfo
{
	char color[48];
	char type[48];
	char logo[48];
	char plate[LPR_PLATE_STR_LEN];
}Info;

int  main(int /*argc*/, char **/*argv*/)
{
  #if 0
	#ifdef NETCOMM_DEBUG_
	update_loglevel(llDebug);
	#else
	int ret = -1;
	BasicPara bp;
	ret = open_syspara_shm();
	if (ret < 0)
	{
		ERR("tcp server open share memory err ret = %d\n", ret);
		SEND_LOG( 1, mdNetComm, llErr, "tcp server open share memory err" );
		return -1;
	}
	ret = read_syspara_node(spBasicPara, &bp);
	if (ret < 0)
	{
		ERR("tcp server read share memory err ret = %d\n", ret);
		SEND_LOG( 1, mdNetComm, llErr, "tcp server read share memory err" );
		close_syspara_shm();
		return -2;
	}
	close_syspara_shm();
	/*Ã‰Ã¨Ã–ÃƒÃˆÃ•Ã–Â¾Ã„Â¬ÃˆÃÃ…Ã¤Ã–ÃƒÂ¼Â¶Â±Ã°*/
	update_loglevel(bp.log_level);
	#endif
	
	CreateThreadObjects();
	StartThreads();
	SetSignalHandler();

	bool  enable_reboot = false;
	enable_reboot = MainThreadLoop();

	TerminateThreads();
	ReleaseResource();
	sync();
	#else

	int server_sock = Socket::CreateTcp();
	if (server_sock < 0)
	{
		ERR("Create Nonblock Tcp failed\n");
		return -1;
	}
	/* ·Ç×èÈûÄ£Ê½ */
	Socket::SetTcpNonblock(server_sock);
	/* °ó¶¨39002¶Ë¿Ú */
	int ret = Socket::Bind(server_sock, 39999);
	if (ret < 0)
	{
		ERR("Bind port 39002 failed\n");
		
		Socket::Close(server_sock);
		return -1;
	}
	/* ¼àÌý¶Ë¿Ú×î¶à5¸ö¿Í»§¶ËÁ¬½Ó */
	ret = Socket::Listen(server_sock, 5);
	if (ret < 0)
	{
		ERR("Listen port failed\n");
		Socket::Close(server_sock);
		return -1;
	}
	
	unsigned char* pbuf	= (unsigned char*)malloc(3312*2352*3);
	
	while (1)
	{
	    char sock_ipaddr[128] ={0};
		/* µÈ´ý¿Í»§¶ËÁ¬½Ó×èÈû2000ms  */
		int clnt_sock = Socket::AcceptByAddr(server_sock, 2000, sock_ipaddr);
		if (clnt_sock < 0)
		{ 
			usleep( 100000 ); 
			printf( "Accept err:%d\n", clnt_sock );
			
			continue; 
		}
		else
		{
			printf("accept tcp sock info:ip_addr:%s\n", sock_ipaddr);
		}
		int count = 0;
		while(1)
		{
			/* ¶ÁÊý¾Ý */
			Data_YUV data = {0};
			int ret = Socket::Readn(clnt_sock, &data, sizeof(Data_YUV), 5000);
			if (ret <= 0)
			{
				printf("remote is not alive\n");
				break;
				
			}
			else if (ret < (int)sizeof(Data_YUV))
			{
				printf("got %d packet, buf too less packet\n",ret);
				break;
				
			}
			else
			{
				
				printf("ParsePacket ret:%d count:%d\n",ret , count++);
			}
			data.buffer = pbuf;
			ret = Socket::Readn(clnt_sock, data.buffer, data.size, 5000);
			if (ret <= 0)
			{
				printf("remote is not alive\n");
				break;
				
			}
			sleep(5);
		    printf("ParsePacket ret:%d size:%d\n",ret , data.size);
            Info info  = {0};
			ret = Socket::Writen(clnt_sock, &info, sizeof(info), 5000);
			if (ret <= 0)
			{
				printf("remote is not alive11111111111\n");
				break;
				
			}
			
		    printf("ParsePacket ret:%d sizeof(info):%d\n",ret , sizeof(info));
			
		}
		/* ´¦ÀíÍê±Ï¾Í¹Ø±ÕÁ¬½Ó */
		Socket::Close(clnt_sock);
		clnt_sock = -1;
		usleep(100000);
	}
	#endif

	return 0;
}

