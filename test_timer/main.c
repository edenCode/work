/*
 * main.c
 *
 *  Created on: 2018年11月1日
 *      Author: gongcm
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <sys/timespec.h>
#include <errno.h>

void  time_thread(void* param)
{
	printf("%s \n",__func__);
}
int main()
{
	int ret = -1;
	pthread_attr_t attr;
	int priority_max;
	int priority_min;

    struct sigevent hard_evp;
    struct itimerspec ts;

	struct sched_param sch_param;
	time_t time;

	ret = pthread_attr_init(&attr);
	if(ret != 0)
	{
		printf("pthread_attr_init failed,ret %d \n",ret);
		return -1;
	}

	ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if(ret != 0)
	{
		printf("pthread_attr_setdetachstate failed,ret %d \n",ret);
		return -1;
	}

	ret = pthread_attr_setschedpolicy(&attr,SCHED_RR); // 设置调度方式
	if(ret != 0)
	{
		printf("pthread_attr_setschedpolicy failed,ret %d \n",ret);
		perror("pthread_attr_setschedpolicy :");
		//return -1;
	}
	priority_max = sched_get_priority_max(SCHED_RR);
	priority_min = sched_get_priority_min(SCHED_RR);

	sch_param.sched_priority = priority_min + 1;
	pthread_attr_setschedparam(&attr,&sch_param);
	if(ret != 0)
	{
		printf("pthread_attr_setschedparam failed,ret %d \n",ret);
		printf("pthread_attr_setschedparam : %s \n",strerrno(ret));
		//return -1;
	}

	memset(&hard_evp,0,sizeof(struct sigevent));

	hard_evp.sigev_notify_function = time_thread;
	hard_evp.sigev_notify = SIGEV_THREAD;
	hard_evp.sigev_notify_attributes = &attr;
	hard_evp.sigev_value.sival_int = 100;

	ret = timer_create(CLOCK_REALTIME,&hard_evp,&time);
	if(ret != 0)
	{
		printf("timer_create failed ,ret %d\n",ret);
		return -3;
	}

	ts.it_interval.tv_sec = 1;
	ts.it_interval.tv_nsec = 0;

	// 第一次间隔5s开始运行。
	ts.it_value.tv_sec = 5;
	ts.it_value.tv_nsec =0;

	ret = timer_settime(time,CLOCK_REALTIME,&ts,NULL);
	if(ret != 0)
	{
		printf("timer_settime failed ,ret %d \n",ret);
		return -5;
	}

	while(1);
	return 0;
}

