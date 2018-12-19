#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void thread_1(void * param1)
{
	int i,j;
	int policy;
	struct sched_param param;
	
	pthread_getschedparam(pthread_self(),&policy,&param);
	

	
	for(i = 0;i < 20; i ++)
	{
		for( j = 0; j < 5000000; j ++)
		{
			
		}
		
		if(policy == SCHED_RR)
		{
			printf("thread_1 tid %ld ,SCHED_RR \n",pthread_self());
		}
		else if(policy == SCHED_FIFO)
		{
			printf("thread_1 tid %ld , SCHED_FIFO \n",pthread_self());
		}
		else
		{
			printf("thread_1 tid %ld , SCHED_OTHER \n",pthread_self());
		}
	}
	
	printf("*********** pthread thread_1 exit *********** \n");
}

void thread_2(void * param1)
{
		int i,j;
	int policy;
	struct sched_param param;
	
	pthread_getschedparam(pthread_self(),&policy,&param);
	

	
	for(i = 0;i < 20; i ++)
	{
		for( j = 0; j < 5000000; j ++)
		{
			
		}
		
		if(policy == SCHED_RR)
		{
			printf("thread_2 tid %ld ,SCHED_RR \n",pthread_self());
		}
		else if(policy == SCHED_FIFO)
		{
			printf("thread_2 tid %ld , SCHED_FIFO \n",pthread_self());
		}
		else
		{
			printf("thread_2 tid %ld , SCHED_OTHER \n",pthread_self());
		}
	}
	
	printf("*********** pthread thread_2 exit *********** \n");
	
}

void thread_3(void * param1)
{
		int i,j;
	int policy;
	struct sched_param param;
	
	pthread_getschedparam(pthread_self(),&policy,&param);
	

	
	for(i = 0;i < 20; i ++)
	{
		for( j = 0; j < 5000000; j ++)
		{
			
		}
		
		if(policy == SCHED_RR)
		{
			printf("thread_3 tid %ld ,SCHED_RR \n",pthread_self());
		}
		else if(policy == SCHED_FIFO)
		{
			printf("thread_3 tid %ld , SCHED_FIFO \n",pthread_self());
		}
		else
		{
			printf("thread_3 tid %ld , SCHED_OTHER \n",pthread_self());
		}
	}
	
	printf("*********** pthread thread_3 exit *********** \n");
}

int main()
{
	int root = getuid();
	if(root == 0)
	{
		printf("Current is  root user \n");
	}
	else
	{
		printf("please change root run this program  \n");
		return -1;
	}
	
	pthread_t ppid1,ppid2,ppid3;
	pthread_attr_t attr,attr1,attr2;
	struct sched_param param;
	
	pthread_attr_init(&attr);
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	
	param.sched_priority = 51;
	pthread_attr_setschedpolicy(&attr1,SCHED_RR);
	pthread_attr_setschedparam(&attr1,&param);
	pthread_attr_setinheritsched(&attr1,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话	
	
	param.sched_priority = 21;
	pthread_attr_setschedpolicy(&attr2,SCHED_RR);
	pthread_attr_setschedparam(&attr2,&param);
	pthread_attr_setinheritsched(&attr2,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
	
	pthread_create(&ppid1,&attr,(void *)thread_1,NULL);
	pthread_create(&ppid2,&attr1,(void *)thread_2,NULL);
	pthread_create(&ppid3,&attr2,(void *)thread_3,NULL);
	
	pthread_join(ppid1,NULL);
	pthread_join(ppid2,NULL);
	pthread_join(ppid3,NULL);
	
	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);
	printf(" __main exit__ \n");
	return 0;
}

/*程序运行结果*/
/**
Current is  root user 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
thread_2 tid 139686211184384 ,SCHED_RR 
*********** pthread thread_2 exit *********** 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
thread_3 tid 139686202791680 ,SCHED_RR 
*********** pthread thread_3 exit *********** 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
thread_1 tid 139686219577088 , SCHED_OTHER 
*********** pthread thread_1 exit *********** 
 __main exit__ 
**/