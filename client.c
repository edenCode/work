#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <sys/statfs.h>


typedef  struct data_{
	int w;
	int h;
	int size;
	int type;
}Data;



typedef struct TagInfo
{
	char color[48];
	char type[48];
	char logo[48];
	char plate[11];
}Info;
//unsigned char buffer[2048 * 1536 * 3];



typedef struct TagResult{
	int classf;
	int confident;
}Result;	

int sendFile(int sockfd,char * file)
{
		
		int retval = -1;
		FILE* fp = NULL;
		int size = -1;	
		unsigned char * buffer = NULL;
		int len = 0;
		unsigned char * temp = NULL;
		
		fp = fopen(file,"r");
		if(!fp)
		{
			printf("error : open %s failed \n",file);
			return -2;	
		}
		
		fseek(fp,0,SEEK_SET);
		
		fseek(fp,0,SEEK_END);
		len = ftell(fp);
		
		rewind(fp);
		
		if(len > 0)
		{
				buffer = (unsigned char *) malloc(len);
				printf("buffer %p size : %d \n",buffer,len);
		}
		
		temp = buffer;
		memset(buffer,0,len);
		
		size = fread(buffer,1,len,fp);
		if(size < 0)
		{
			perror("fread file \n");
			return -4;	
		}
		printf("fread size %d \n",size);
		
		if(fp)
			fclose(fp);
		
		while(size > 0){
				retval = send(sockfd,buffer,size,0);
				printf("send %d bytes,\n",retval);
				if(retval < 0)
				{
					perror("send socket : ");
					printf(" wait send data \n");
					continue;	
				}
				
				size -= retval;
				buffer += retval;
				
		}

		if(temp)
			free(temp);
		return len;
}


int recvResult(int sockfd)
{
	int retval = 0;
	//Result result;
	Info info;
	if(sockfd < 0)
	{
		printf(" sockfd error");
		return -1;
	}
	

	retval = recv(sockfd,&info,sizeof(info),0);
	printf("recv %d bytes  ...  \n",retval);	
	printf("color %s,type %s,logo %s ,plate %s \n",info.color,info.type,info.logo,info.plate);
	return retval;
}

int getFileSize(char * file)
{
	FILE * fp = NULL;
	int  len = 0;
	
	fp = fopen(file,"r");
	if(!fp)
	{
		printf("error : open %s failed \n",file);
		return -2;	
	}
		
	fseek(fp,0,SEEK_SET);
		
	fseek(fp,0,SEEK_END);
	len = ftell(fp);

	fclose(fp);

	printf("file size  %d  bytes \n",len);
	return len;		//rewind(fp);	
}

int parsefile(char * file)
{

}

int sendPacked(int sockfd,char * name)
{
	Data data;
	int size = 0;

	data.w = 2048;
 	data.h = 1536;
 	data.size = getFileSize(name);
 	data.type = 1;
 	
	//while(1){
	size = send(sockfd,&data,sizeof(data),0);
	printf("send %d bytes ... \n",size);
	
	usleep(300);
	size += sendFile(sockfd,name);
	printf("send %d bytes ... \n",size);
	
	size = recvResult(sockfd);
	printf("recv %d bytes ... \n",size);
}

int sendAuto(int sockfd,char * path)
{
	DIR * dir = NULL;
	struct dirent * file;

	dir = opendir(path);
	if(dir == NULL)
	{
		printf("opendir Failed \n");
		return -1;
	}

	while((file = readdir(dir)) != NULL)
	{
		if(fnmatch(".",file->d_name,0) == 0 || fnmatch("..",file->d_name,0) == 0)
		{
			continue;
		}

		if(file->d_type == DT_DIR)
		{
			continue;
		}

		if(strstr(file->d_name,".yuv"))
		{
			sendPacked(sockfd,file->d_name);
		}
	}
}

int main(int argc,char * argv[])
{
	int sockfd = -1;
	char hostname[40];
	struct sockaddr_in saddr;
	struct in_addr iaddr;
  struct hostent *host = NULL;
  int size = 0;
  Data data;
  
  //gethostname(hostname,sizeof(hostname));
 	host = gethostbyname("www.baidu.com");
 	if(host == NULL)
 	{
 		printf("host error \n");
 		return -1;	
 	}
 	
 	printf("\n\n host name : %s \n\n",host->h_name);
 	
 	if(argc  < 4)
 	{
 		printf("Usage : ./main ip port filename \neg    : ./main 192.168.11.239 39999 1244X12333.yuv \n");
 		return -1;	
 	}
 	
 	printf(" ip   %s  \n prot %s  \n File %s \n",argv[1],argv[2],argv[3]);
 	
 	sockfd=socket(AF_INET,SOCK_STREAM,0);
 	if(sockfd < 0)
 	{
 		printf("error : socket create failed \n");
 		return -2;
 	}
 	
 	bzero(&saddr,sizeof(saddr));
 	saddr.sin_family=AF_INET;
 	saddr.sin_port=htons(atoi(argv[2]));
 	saddr.sin_addr.s_addr=inet_addr(argv[1]);
 	
 	if(connect(sockfd,(struct sockaddr *)&saddr,sizeof(struct sockaddr)) < 0)
 	{
 		perror("Connected");
 		printf("erro : connect failed \n");	
 		return -3;
 	} 
 	
 	printf("server ip:%s, newSocket = %d.\n", inet_ntoa(saddr.sin_addr), sockfd);

 	data.w = 2048;
 	data.h = 1536;
 	data.size = getFileSize(argv[3]);
 	data.type = 1;
 	
	while(1){
		size = send(sockfd,&data,sizeof(data),0);
		printf("send %d bytes ... \n",size);
	
		
		size = sendFile(sockfd,argv[3]);
		printf("send %d bytes ... \n",size);
		usleep(300);
		size = recvResult(sockfd);
		printf("recv %d bytes ... \n",size);
		usleep(100);
	}
		if(sockfd > 0)
			close(sockfd);
 	return 0;
}

