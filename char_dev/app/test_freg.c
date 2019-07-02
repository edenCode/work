#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void read_thread(void *arg)
{
    int ret = -1;
    int i = 0;
    int fd = *((int *)arg);
    while(1){
        ret = read(fd,&i,sizeof(i));
        printf("read ret = %d ,i = %d ",ret,i);
        if(i == 5)
            break;
    }
}

void write_thread(void *arg)
{
    int ret = -1;
    int i = 0;
    int fd = *((int *)arg);
    while(1){
        ret = write(fd,&i,sizeof(i));
        printf("write ret = %d,i = %d \n",ret,i);
        sleep(1);
        if(i == 5)
         break;
    }
    
}
int main(int argc, char const *argv[])
{
    pthread_t tid1,tid2;
    int fd = -1;
    fd = open("/dev/freg",O_RDWR);
    if(fd < 0)
    {
        printf(" open failed /dev/freg \n");
        return -1;
    }

    pthread_create(&tid1,NULL,write_thread,fd);
    pthread_create(&tid2,NULL,read_thread,fd);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    if(fd)
        close(fd);
    return 0;
}
