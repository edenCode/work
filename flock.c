#include <stdio.h>
#include <fcntl.h>
#define read_lock(fd,offset,whence,len) lock_register((fd),F_SETLK,F_RDLCK,(offset),(whence),(len))
#define read_lockw(fd,offset,whence,len) lock_register((fd),F_SETLKW,F_RDLCK,(offset),(whence),(len))
#define write_lock(fd,offset,whence,len) lock_register((fd),F_SETLK,F_WRLCK,(offset),(whence),(len))
#define write_lockw(fd,offset,whence,len) lock_register((fd),F_SETLKW,F_WRLCK,(offset),(whence),(len))

#define un_lock(fd,offset,whence,len) lock_register((fd),F_SETLKW,F_UNLCK,(offset),(whence),(len))
int main()
{
	int fd = -1;
	pid_t pid;
		
	fd  = open("./test",O_RDWR);	
	printf("%d child and parent union \n",getpid());
	pid = fork();
	
	printf("This is pid £º %d \n",getpid());
	if(pid < 0)
	{
		return -1;	
	}

	if(pid == 0)
	{
		printf("wait .... \n");
		int val = read_lockw(fd,0,SEEK_CUR,0);
		if(val == 0)
		{
			printf("success pid : %d \n",getpid());	
		}else
		{
			printf("Failed pid : %d \n",getpid());	
		}
			
		printf("child process pid :  %d ,ppid : %d\n",getpid(),getppid());	
		
	}else{
		printf("parent wait for child exit .... \n");
		int val = read_lockw(fd,0,SEEK_CUR,0);
		printf("parent process pid : %d \n",getpid());	
		sleep(1);
		getchar();
	}
	
	printf(" exit program current pid : %d \n",getpid());
	return 0;
}

int lock_register(int fd,int cmd,int type,off_t offset,int whence,off_t len)
{
	int ret;
	struct flock lock;
	
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;
	
	ret = fcntl(fd,cmd,&lock);
	return ret;
}