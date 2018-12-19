#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>

#define SENDSIGTIME 10
 
pthread_cond_t g_cond;
pthread_mutex_t g_mutex;
 
void thread1(void *arg)
{
    int inArg = (int)arg;
    int ret = 0;
	int time;
    struct timeval now;
	struct timeval now1;
    struct timespec outtime;
 
    pthread_mutex_lock(&g_mutex);
 
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
	printf("pthread_cond_timedwait ... \n");
    ret = pthread_cond_timedwait(&g_cond, &g_mutex, &outtime);
	gettimeofday(&now1, NULL);
	
	time = now1.tv_sec - now.tv_sec;
    //ret = pthread_cond_wait(&g_cond, &g_mutex);
	printf("pthread_cond_timedwait,return %d ,time %d\n",ret,time);
    pthread_mutex_unlock(&g_mutex);
 
    printf("thread 1 ret: %d\n", ret);
 
}
 
int main(void)
{
    pthread_t id1;
    int ret;
 
    pthread_cond_init(&g_cond, NULL);
    pthread_mutex_init(&g_mutex, NULL);
 
    ret = pthread_create(&id1, NULL, (void *)thread1, (void *)1);
    if (0 != ret)
    {
	printf("thread 1 create failed!\n");
	return 1;
    }
	printf("ETIMEDOUT %d,EPERM %d,EINVAL %d \n",ETIMEDOUT,EPERM,EINVAL);
    printf("等待%ds发送信号!\n", SENDSIGTIME);
    sleep(SENDSIGTIME);
    printf("正在发送信号....\n");
    pthread_mutex_lock(&g_mutex);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
 
 
    pthread_join(id1, NULL);
 
    pthread_cond_destroy(&g_cond);
    pthread_mutex_destroy(&g_mutex);
 
    return 0;
}
