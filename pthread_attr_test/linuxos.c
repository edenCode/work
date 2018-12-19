#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sllLib.h>
#include <dllLib.h>
#include <osal.h>

/*
// Osal_Init - Initializes OSAL Library.
//
// RETURNS: 0: Success, -1: failed.
*/
int Osal_Init( int(*init)(void*), void* arg, void(*cleanup)(void) )
{
	struct sched_param sp;
	
    /* 1. Lock all process memory to prevent swapping out. */
	if(mlockall(MCL_CURRENT | MCL_FUTURE)) return (-1);

    /* 2. Set current process to max priority. */
	if(setpriority(PRIO_PROCESS, 0, -20)) return (-1);

    /* 3. Use real-time schedule policy and set parameters. */
	memset(&sp, 0, sizeof(struct sched_param));
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &sp)==-1)
		return (-1);

    /* 4. Call initialize entry. */
	if((init != NULL) && ((*init)(arg))) return (-1);

    /* 5. Register exit routine. */
	if((cleanup!=NULL) && (atexit(cleanup)!=0)) return (-1);

    return 0;
}

/* 
// Osal_Delay - Delay time in ms.
//
// RETURNS: 0: Success, -1: failed.
*/
int Osal_Delay( int tminms )
{
	struct timespec delayTime, elaspedTime;

	/* Otherwise delay for specific time */
	delayTime.tv_sec  = tminms / 1000;
	delayTime.tv_nsec = ( tminms % 1000 ) * 1000000;

	return nanosleep(&delayTime, &elaspedTime);
}

/* Event Object. */
typedef struct os_event {
	int           wanted; /* event to be wanted */
	int         received; /* event already received */
	pthread_mutex_t lock; /* Mutex object */
	pthread_cond_t  cond; /* condition variable */
} OS_EVENT;

/*
// OSEvent_Create - Create a OS Event object.
//
// RETURNS: Handle to Event, NULL if failed.
*/
OSEVENT_HANDLE OSEvent_Create( int wanted )
{
	OSEVENT_HANDLE hEvent;
	int ret = 0;

	/* 1. Allocate memory for OS Event object */
	hEvent = (OSEVENT_HANDLE)calloc(1, sizeof(OS_EVENT));
	if (hEvent==NULL) return NULL;
	
	/* 2. Initialize mutex object */
    ret  = pthread_mutex_init(&hEvent->lock, NULL);
	ret |= pthread_cond_init (&hEvent->cond, NULL);
	if (ret) {
		free((char*)hEvent);
        return NULL;
    }
	
	/* 3. Initialize signal value */
	hEvent->wanted   = wanted;
	hEvent->received = 0;

    return hEvent;
}

/*
// OSEvent_Delete - Delete the Event.
//
// RETURNS: 0 if success, otherwise -1.
*/
int OSEvent_Delete( OSEVENT_HANDLE hEvent )
{
	if (hEvent==NULL) return -1;
	
	/* 1. Clear signal */
	hEvent->wanted   = 0;
	hEvent->received = 0;
	
	/* 2. Destroy objects */
	pthread_cond_destroy (&hEvent->cond);
	pthread_mutex_destroy(&hEvent->lock);

	/* 3. Free Event object memory */
	free((char*)hEvent);

	return 0;
}

/* 
// OSEvent_Take - wait a Event flag(s) in specified time.
//
// RETURNS: 0 if success, otherwize -1.
*/
int OSEvent_Take( OSEVENT_HANDLE hEvent, int mode, int tminms )
{
	int found, ret = 0;

	if (hEvent==NULL) return -1; 
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hEvent->lock);
	pthread_mutex_lock(&hEvent->lock);
	while (1) {
		found = 0;
		if((mode & OSEVENT_WAIT_MASK) == OSEVENT_WAIT_ALL) {
			/* Wait for all */
			if(hEvent->wanted == hEvent->received) {
                found = 1;
            }
		} else {
			/* Wait for any */
			if(hEvent->wanted & hEvent->received) {
                found = 1;
            }
		}
		if(found) {
			if(mode & OSEVENT_MODE_CLR) {
				/* Clear after received */
				hEvent->received &= ~(hEvent->wanted & hEvent->received);
			}
			break;
		} else {
			/* No event occurs */
			if(tminms == NO_WAIT) {
				/* No wait mode */
				ret = -1;
			} else if (tminms == WAIT_FOREVER) {
				/* Wait forever */
				ret = pthread_cond_wait(&hEvent->cond, &hEvent->lock);
			} else {
				struct timespec abstm;

				/* Timed wait */
		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        ret = pthread_cond_timedwait(&hEvent->cond, &hEvent->lock, &abstm);
			}
		    if (ret) break;
		}
	}
	pthread_mutex_unlock(&hEvent->lock);
	pthread_cleanup_pop(0);

	return (ret ? (-1) : 0);
}

/* 
// OSEvent_Give - Set the Event flag(s)
//
// RETURNS: 0 if success, otherwise -1.
*/
int OSEvent_Give( OSEVENT_HANDLE hEvent, int event )
{
	int ret = 0;

	if (hEvent==NULL) return -1; 
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hEvent->lock);
	pthread_mutex_lock(&hEvent->lock);
	hEvent->received |= event;
	ret = pthread_cond_signal(&hEvent->cond);
	pthread_mutex_unlock(&hEvent->lock);
	pthread_cleanup_pop(0);

	return ret ? (-1) : 0;
}

/* 
// OSEvent_Clear - Clear the Event flag(s)
//
// RETURNS: 0 if success, otherwise -1.
*/
int OSEvent_Clear( OSEVENT_HANDLE hEvent, int event )
{
	if (hEvent==NULL) return -1; 
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hEvent->lock);
	pthread_mutex_lock(&hEvent->lock);
	hEvent->received &= ~event;
	pthread_mutex_unlock(&hEvent->lock);
	pthread_cleanup_pop(0);

	return (0);
}

/* 
// OSEvent_Given - Check if the specific event is given.
//
// RETURNS: 0 if success, otherwise -1.
*/
int OSEvent_Given( OSEVENT_HANDLE hEvent, int event )
{
	int done = 0;

	if (hEvent==NULL) return 0; 
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hEvent->lock);
	pthread_mutex_lock(&hEvent->lock);
	done = hEvent->received & event;
	pthread_mutex_unlock(&hEvent->lock);
	pthread_cleanup_pop(0);

	return !!done;
}

/* Mutex definition */
typedef struct os_mutex
{
	pthread_mutex_t lock; /* Mutex object */
} OS_MUTEX;

/*
// OSMutex_Create - Create a Mutex object.
//
// RETURNS: 0 if success, otherwize -1.
*/
OSMUTEX_HANDLE OSMutex_Create( void )
{
	OSMUTEX_HANDLE hMutex;

	hMutex = (OSMUTEX_HANDLE)calloc(1, sizeof(OS_MUTEX));
	if (hMutex==NULL) return (NULL);
	
	if (pthread_mutex_init(&hMutex->lock, NULL)) {
		free (hMutex);
		return (NULL);
	}

	return (hMutex);
}

/*
// OSMutex_Delete - Delete the Mutex.
//
// RETURNS: 0 if success, otherwise -1.
*/
int OSMutex_Delete( OSMUTEX_HANDLE hMutex )
{
	if (hMutex==NULL) return -1;
	
	if (pthread_mutex_destroy(&hMutex->lock)) return -1;
	
	free (hMutex);

	return 0;
}

/*
// OSMutex_Take - Lock the Mutex.
//
// RETURNS: 0-success, otherwize -1
*/
int OSMutex_Take( OSMUTEX_HANDLE hMutex, int tminms )
{
	int ret = 0;

	if (hMutex==NULL) return -1;
  	if (tminms==0) {
  		ret = pthread_mutex_trylock(&hMutex->lock);
	} else if (tminms==-1) {
  		ret = pthread_mutex_lock(&hMutex->lock);
	} else {
		struct timespec abstm;

		clock_gettime(CLOCK_REALTIME, &abstm);
		abstm.tv_sec  += (tminms / 1000);
		abstm.tv_nsec += (tminms % 1000 * 1000000);
		ret = pthread_mutex_timedlock(&hMutex->lock, &abstm);
	}
	
	return ret ? -1 : 0;
}

/*
// OSMutex_Give - Release the Mutex object.
//
// RETURNS: 0-success, otherwise -1.
*/
int OSMutex_Give( OSMUTEX_HANDLE hMutex )
{
	if (hMutex==NULL) return -1;
	
	return pthread_mutex_unlock(&hMutex->lock) ? -1 : 0;
}

/*
// Semaphore definition. 
*/
typedef struct os_sem
{
	int            count; /* Semaphore count */
	int         maxCount; /* Max Semaphore count */
    pthread_mutex_t lock; /* Mutex object */
	pthread_cond_t  cond; /* Condition variable */
} OS_SEM;

/*
// OSSem_Create - Create a semaphore object.
//
// RETURNS: semaphore handle, otherwise NULL.
*/
OSSEM_HANDLE OSSem_Create( int maxCount, int initVal )
{
	OSSEM_HANDLE hSem  = NULL;
	int ret = 0;

	hSem = (OSSEM_HANDLE)calloc(1, sizeof(OS_SEM));
    if (hSem==NULL) return (NULL);
	
    ret |= pthread_mutex_init(&hSem->lock, NULL);
	ret |= pthread_cond_init (&hSem->cond, NULL);
	if (ret) {
		free (hSem);
        return (NULL);
    }
	hSem->maxCount = maxCount;
	hSem->count    = initVal;
	if (0==hSem->maxCount) 
		hSem->maxCount = 1;
	if (hSem->count>hSem->maxCount) 
		hSem->count = hSem->maxCount;
	
    return (hSem);
}

/*
// OSSem_Delete - Delete the semaphore, and release the related rsrc.
//
// RETURNS: 0-success, otherwize -1.
*/
int OSSem_Delete( OSSEM_HANDLE hSem )
{
	if (hSem==NULL) return -1;
	
    pthread_mutex_destroy(&hSem->lock);
	pthread_cond_destroy (&hSem->cond);
	free (hSem);

	return (0);
}

/*
// OSSem_Take - Wait the semaphore.
//
// RETURNS: 0-success, otherwize -1
*/
int OSSem_Take( OSSEM_HANDLE hSem, int tminms )
{
	int ret = 0;
	
	if (hSem==NULL) return -1;
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hSem->lock);
    pthread_mutex_lock(&hSem->lock);
	while(1) {
  		if (hSem->count>0) {
    		hSem->count--;
      		ret = 0;
      		break;
    	} else {
      		if (tminms==0) 
      			ret = -1;
			else if (tminms==-1) 
				ret = pthread_cond_wait(&hSem->cond, &hSem->lock);
			else {
				struct timespec abstm;

		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        ret = pthread_cond_timedwait(&hSem->cond, &hSem->lock, &abstm);
			}
		    if (ret) break;
    	}
  	}
	pthread_mutex_unlock(&hSem->lock);
	pthread_cleanup_pop(0);
	return ret ? (-1) : 0;
}

/*
// OSSem_Give - Post the semaphore.
//
// RETURNS: 0-success, otherwize -1
*/
int OSSem_Give( OSSEM_HANDLE hSem )
{
	int ret = 0;
	
	if (hSem==NULL) return -1;
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hSem->lock);
    pthread_mutex_lock(&hSem->lock);
	if (hSem->count < hSem->maxCount) {
    	hSem->count++;
    	ret |= pthread_cond_signal(&hSem->cond);
  	}
	pthread_mutex_unlock(&hSem->lock);
	pthread_cleanup_pop(0);	

	return ret ? (-1) : 0;
}

/* 
// OSSem_Count - Get semaphore count.
//
// RETURNS: count of current semaphore.
*/
int OSSem_Count( OSSEM_HANDLE hSem )
{
	int count = -1;
	
	if (hSem==NULL) return -1;
	
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hSem->lock);
    pthread_mutex_lock(&hSem->lock);
    count = hSem->count;
	pthread_mutex_unlock(&hSem->lock);
	pthread_cleanup_pop(0);

    return count;
}

/* Message queue definition */
typedef struct os_msgq
{
	SL_LIST        qReady; /* message queue head */
	SL_LIST         qFree; /* free message queue head */
	void*         msgPool; /* messages pool */
	int	        maxMsgCnt; /* max number of messages in queue */
	int	        maxMsgLen; /* max length of message */
	pthread_mutex_t  lock; /* mutex */
	pthread_cond_t condRd; /* condition variable for pending on reading */
	pthread_cond_t condWr; /* condition variable for pending on writing */
} OS_MSGQ;

/* message node typedefs */
typedef struct {
    SL_NODE node; /* queue node */
    int	  msgLen; /* number of bytes of data */
} MSG_NODE;

#define MSG_NODE_DATA(pNode) (((char*)pNode) + sizeof(MSG_NODE))

/* The following macro determines the number of bytes needed to buffer
 * a message of the specified length.  The node size is rounded up for
 * efficiency.  The total buffer space required for a pool for
 * <maxMsgCnt> messages each of up to <maxMsgLen> bytes is:
 *
 *    maxMsgCnt * MSG_NODE_SIZE (msgLen)
 */
#define MSG_NODE_SIZE(msgLen) \
	(ROUND_UP((sizeof (MSG_NODE) + msgLen), sizeof(void*)))

/*
// OSMsgQ_Create - Create and initialize a message queue.
//
// This routine inits a message queue capable of holding up to <maxMsgCnt>
// messages, each up to <maxMsgLen> bytes long.  The routine returns 
// a message queue ID used to identify the initd message queue in all 
// subsequent calls to routines in this library. 
//
// RETURNS: 0 if success, otherwize -1.
*/
OSMSGQ_HANDLE OSMsgQ_Create( int maxMsgCnt, int maxMsgLen )
{
	OSMSGQ_HANDLE hMsgQ;
    void* pool   = NULL;
    int nodeSize = MSG_NODE_SIZE (maxMsgLen);
	int size, ix, ret = 0;

	/* 1. Create msg q Object */
	hMsgQ = (OSMSGQ_HANDLE)calloc(1, sizeof(OS_MSGQ));
	if (hMsgQ==NULL) return NULL;
	
	ret |= pthread_mutex_init(&hMsgQ->lock,   NULL);
	ret |= pthread_cond_init (&hMsgQ->condRd, NULL);
	ret |= pthread_cond_init (&hMsgQ->condWr, NULL); 
	if (ret) {
		free (hMsgQ);
		return NULL;
	}
	
    /* 2. Allocate messages pool */
    if (maxMsgCnt<=0) maxMsgCnt = 16;
    if (maxMsgLen<=0) maxMsgLen = 128;
    size = (int) maxMsgCnt * MSG_NODE_SIZE(maxMsgLen);
	pool = (void*)malloc(size);
	if (pool==NULL) {
		free (hMsgQ);
		return NULL;
	}
	
    /* 3. Initialize internal message queues */
	SLL_INIT(&hMsgQ->qReady);
	SLL_INIT(&hMsgQ->qFree );
	hMsgQ->msgPool = pool;
	
    /* 4. Put msg nodes on free list */
    for (ix=0; ix<maxMsgCnt; ix++) {
		sllPutAtTail( &hMsgQ->qFree, (SL_NODE*)pool);
		pool = (void*)(((char*)pool) + nodeSize);
	}

    /* 5. Initialize rest of msg q */
	hMsgQ->maxMsgCnt = maxMsgCnt;
	hMsgQ->maxMsgLen = maxMsgLen;

	return (hMsgQ);
}

/*
// OSMsgQ_Delete - cleanup a message queue.
//
// RETURNS: 0 if success, otherwize -1.
*/
int OSMsgQ_Delete( OSMSGQ_HANDLE hMsgQ )
{
	if (hMsgQ == NULL) return (-1);

	if(hMsgQ->msgPool != NULL) {
		free(hMsgQ->msgPool);
	}
	pthread_mutex_destroy(&hMsgQ->lock); 
	pthread_cond_destroy (&hMsgQ->condRd);
	pthread_cond_destroy (&hMsgQ->condWr);
	
	free (hMsgQ);
	
	return 0;
}

/*
// OSMsgQ_Send - send a message to a message queue.
//
// RETURNS: number of bytes received, or -1 if failed.
*/
int OSMsgQ_Send( OSMSGQ_HANDLE hMsgQ, char *buffer, int nbytes, int tminms, int priority )
{
    MSG_NODE* pMsg;
	int ret = 0;

	if ((hMsgQ == NULL) || 
	    (nbytes>hMsgQ->maxMsgLen)) return -1;
		
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hMsgQ->lock);
	pthread_mutex_lock(&hMsgQ->lock);
	while (1) 
	{
		pMsg = (MSG_NODE*)sllGet( &hMsgQ->qFree);
		if (pMsg == NULL) 
		{
			if( 0 == tminms ) 
			{
				ret = -1;
			}
			else if ( -1 == tminms ) 
			{
				/* 这里等待处理 */
				ret = pthread_cond_wait(&hMsgQ->condWr, &hMsgQ->lock);
			}
			else
			{ 
                /* Timed wait */
				struct timespec abstm;

		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        ret = pthread_cond_timedwait(&hMsgQ->condWr, &hMsgQ->lock, &abstm);
			}
		    if (ret) 
			{
				break;
		    }
		} 
		else 
		{
			memcpy(MSG_NODE_DATA(pMsg), buffer, nbytes);
			if (priority!=OSMSG_PRI_NORMAL) 
			{
				sllPutAtHead(&hMsgQ->qReady, (SL_NODE*)pMsg);
			}
			else 
			{
				sllPutAtTail(&hMsgQ->qReady, (SL_NODE*)pMsg);
			}
			pMsg->msgLen = nbytes;
			ret = pthread_cond_signal(&hMsgQ->condRd);
			break;
		}
	}
	pthread_mutex_unlock(&hMsgQ->lock);
	pthread_cleanup_pop(0);

	return (ret ? (-1) : nbytes);
}

/*
// OSMsgQ_Receive - receive a message from a message queue.
//
// This routine receives a message from the message queue <hMsgQ>.
// The received message is copied into the specified <buffer>, which is
// <maxNBytes> in length.  If the message is longer than <maxNBytes>,
// the remainder of the message is discarded (no error indication
// is returned).
//
// RETURNS: number of bytes received, or -1 if failed.
*/
int OSMsgQ_Receive( OSMSGQ_HANDLE hMsgQ, char *buffer, int maxnbytes, int tminms )
{
    MSG_NODE* pMsg;
    int nret = 0, ret = 0;

	if (hMsgQ==NULL) return -1;
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hMsgQ->lock);
	pthread_mutex_lock(&hMsgQ->lock);
	while (1) {
		pMsg = (MSG_NODE*)sllGet(&hMsgQ->qReady);
		if (pMsg == NULL) {
			if(tminms==0)	
				ret = -1;
			else if (tminms==-1) 
				ret = pthread_cond_wait(&hMsgQ->condRd, &hMsgQ->lock);
			else {
				struct timespec abstm;
				
		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        ret = pthread_cond_timedwait(&hMsgQ->condRd, &hMsgQ->lock, &abstm);
			}
		    if (ret) break;
		} else {
			nret = MIN(maxnbytes, pMsg->msgLen);
			memcpy(buffer, MSG_NODE_DATA(pMsg), nret);
			sllPutAtTail(&hMsgQ->qFree, (SL_NODE*)pMsg);
			ret = pthread_cond_signal(&hMsgQ->condWr);
			break;
		}
	}
	pthread_mutex_unlock(&hMsgQ->lock);
	pthread_cleanup_pop(0);

	return (ret ? (-1) : nret);
}

/*
// OSMsgQ_Count - get the number of messages queued to a message queue
//
// RETURNS: The number of messages queued, or -1.
*/
int OSMsgQ_Count( OSMSGQ_HANDLE hMsgQ )
{
	int count = 0;

	if (hMsgQ==NULL) return 0;
	
	pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&hMsgQ->lock);
	pthread_mutex_lock(&hMsgQ->lock);
	count = sllCount( &hMsgQ->qReady );
	pthread_mutex_unlock(&hMsgQ->lock);
	pthread_cleanup_pop(0);

	return count;
}

/*
// Task Object.
*/
typedef struct os_task 
{
    DL_NODE    node;   /* Base node of Task */
    int       state;   /* Task state */
	TASK_FXN  entry;   /* Task entry point */
	pthread_t thrid;   /* pthread identifier */
	void*   environ;   /* Task environment pointer */
} OS_TASK;

/*
// Task List.
*/
static struct
{
    DL_LIST     qTaskHead; /* list of all tasks */
	BOOL           bReady; /* Initialize status */
	pthread_mutex_t mutex; /* list access guard */
} gTaskGlobalInfo;

/*
// initRicTaskGlobalInfo - Initializes gTaskGlobalInfo.
//
// RETURNS: N/A.
*/
static void initTaskGlobalInfo( void )
{
    DLL_INIT(&gTaskGlobalInfo.qTaskHead);
    pthread_mutex_init(&gTaskGlobalInfo.mutex, NULL);
	gTaskGlobalInfo.bReady = TRUE;
}

/*
// registerTask - Registers task to gTaskGlobalInfo.
//
// RETURNS: N/A.
*/
static void registerTask( OSTASK_HANDLE me )
{
	OSTASK_HANDLE hTask = NULL;

	if (!gTaskGlobalInfo.bReady) {
		/* Initialize task list */
		initTaskGlobalInfo();
	}
	
	/* Add task to task list */
	(void)pthread_mutex_lock(&gTaskGlobalInfo.mutex);
	
	/* Check if task existed in list? */
	hTask = (OSTASK_HANDLE)DLL_FIRST(&gTaskGlobalInfo.qTaskHead);
	while (hTask!=NULL) {
		if (OSTask_GetId(hTask)==OSTask_GetId(me)) break;
		hTask = (OSTASK_HANDLE)DLL_NEXT((DL_NODE*)hTask);
	}
	
	/* If not exist in the task list, add it */
	if (hTask==NULL) 
		dllAdd(&gTaskGlobalInfo.qTaskHead, (DL_NODE*)me);
	
	(void)pthread_mutex_unlock(&gTaskGlobalInfo.mutex);
}

/*
// deregisterTask - Deregister task from gTaskGlobalInfo.
//
// RETURNS: N/A.
*/
static void deregisterTask( OSTASK_HANDLE me )
{
	OSTASK_HANDLE hTask = NULL;

	/* Delete task from task list */
	(void)pthread_mutex_lock(&gTaskGlobalInfo.mutex);
	
	/* Check if task existed in list? */
	hTask = (OSTASK_HANDLE)DLL_FIRST(&gTaskGlobalInfo.qTaskHead);
	while (hTask!=NULL) {
		if (OSTask_GetId(hTask)==OSTask_GetId(me)) break;
		hTask = (OSTASK_HANDLE)DLL_NEXT((DL_NODE*)hTask);
	}
	
	/* If exist in the task list, remove it */
	if (hTask!=NULL) 
		dllRemove(&gTaskGlobalInfo.qTaskHead, (DL_NODE*)me);
	
	(void)pthread_mutex_unlock(&gTaskGlobalInfo.mutex);
}

/*
// doExecute - Task callback routine 
*/
static void* doExecute( void* arg )
{
    OSTASK_HANDLE hTask = (OSTASK_HANDLE)arg;

    /* 1. Set task local errno to 0 */
    errno = 0;
	
    /* 2. Set Cancel to async type */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
    /* 3. Call user routine */
    return (*hTask->entry)(hTask);
}

/*
// OSTask_Create - Create a task object with specified attribute.
//
// RETURNS: Task handle, otherwize -1.
*/
OSTASK_HANDLE OSTask_Create( int priority, char* stackBase, int stackSize, TASK_FXN entry )
{
	OSTASK_HANDLE hTask = NULL;
    pthread_attr_t thrattr;
	struct sched_param param;
	int ret = 0;

	/* 1. Allocate memory for task object */
	hTask = (OSTASK_HANDLE)calloc(1, sizeof(OS_TASK));
    if (hTask==NULL) return (NULL);
	hTask->state = 0;
	hTask->thrid = 0;
	hTask->entry = entry;
	hTask->environ = NULL;

	/* 2. Init pthread attributes */
	if (pthread_attr_init(&thrattr)) {
		free (hTask);
		return (NULL);
	}
	ret  = pthread_attr_setinheritsched(&thrattr, PTHREAD_EXPLICIT_SCHED);
	ret |= pthread_attr_setschedpolicy(&thrattr, SCHED_FIFO);
	ret |= pthread_attr_setscope(&thrattr, PTHREAD_SCOPE_SYSTEM);
	if (priority>sched_get_priority_max(SCHED_FIFO)) 
        priority = sched_get_priority_max(SCHED_FIFO);
	if (priority<sched_get_priority_min(SCHED_FIFO)) 
        priority = sched_get_priority_min(SCHED_FIFO);
	param.sched_priority = priority;
	ret |= pthread_attr_setschedparam(&thrattr, &param);
	if (stackSize!=0) 
    	ret |= pthread_attr_setstacksize(&thrattr, stackSize);
	
	/* 3. Create thread and execute it */
    ret |= pthread_create(&hTask->thrid, &thrattr, doExecute, (void*)hTask);
	if (ret) {
		free (hTask);
		return (NULL);
	}
	/* 4. Register the thread to tasks list */
	registerTask(hTask);

    return (hTask);
}

/*
// OSTask_Delete - Delete the task and release assotiate resources. 
//  
// RETURNS: 0: Sucess, -1: failed.
*/
int OSTask_Delete( OSTASK_HANDLE hTask )
{
	OSTASK_HANDLE hCurTask;
	int ret = 0;

	/* 1. If hTask is NULL, delete self */
	hCurTask = (hTask==NULL) ? OSTask_Self() : hTask;

	/* 2. For main task, We cannt delete it */
	if (hCurTask == NULL) return 0;
	pthread_cleanup_push((void(*)(void*))free, (void*)hCurTask);
	deregisterTask(hCurTask);

	/* 3.1. Exit self */
	if (hCurTask->thrid==pthread_self()) 
		pthread_exit((void*)0);
	/* 3.2. Exit other thread */
	else { 
		ret  = pthread_cancel(hCurTask->thrid);
		ret |= pthread_join(hCurTask->thrid, (void**)NULL);
	}
	pthread_cleanup_pop(1);

	return ret ? (-1) : 0;
}

/* 
// OSTask_SetPriority - Set priority of the task.
//
// RETURNS: 0: Success, -1: failed.
*/
int OSTask_SetPriority( OSTASK_HANDLE hTask, int priority )
{
	struct sched_param param;
	pthread_t thrid;
	int policy, ret = 0;

	/* 1. If hTask is NULL, get current calling id */
	thrid = (hTask==NULL) ? pthread_self() : hTask->thrid;

	/* 2. Get the supported min and max priority range */
	if (priority>sched_get_priority_max(SCHED_FIFO)) 
        priority = sched_get_priority_max(SCHED_FIFO);
	if (priority<sched_get_priority_min(SCHED_FIFO)) 
        priority = sched_get_priority_min(SCHED_FIFO);
	
	/* 3. Set the priority */
	ret = pthread_getschedparam(thrid, &policy, &param);
	param.sched_priority = priority;
	ret |= pthread_setschedparam(thrid, policy, &param);

	return ret ? (-1) : 0;
}

/* 
// OSTask_GetPriority - Get priority of the task.
//
// RETURNS: Priority of task, -1:failed.
*/
int OSTask_GetPriority( OSTASK_HANDLE hTask )
{
	struct sched_param param;
	pthread_t thrid;
	int policy, ret = 0;

    /* If hTask is NULL, get current calling id */
	thrid = (hTask==NULL) ? pthread_self() : hTask->thrid;
	
	/* Get task schedule parameters */
	ret = pthread_getschedparam(thrid, &policy, &param);

	return ret ? (-1) : param.sched_priority;
}

/* 
// OSTask_SetState - Set state of the task.
//
// RETURNS: 0: Sucess, -1: failed.
*/
int OSTask_SetState( OSTASK_HANDLE hTask, int state )
{
	if (hTask==NULL) return -1;

	hTask->state = state;

	return 0;
}

/* 
// OSTask_GetState - Get state of the task.
//
// RETURNS: Current state of task.
*/
int OSTask_GetState( OSTASK_HANDLE hTask )
{
	if (hTask==NULL) return 0;

	return hTask->state;
}

/* 
// OSTask_GetEnv - Get environment of the task.
//
// RETURNS: Current state of task.
*/
void* OSTask_GetEnv( OSTASK_HANDLE hTask )
{
	if (hTask==NULL) return NULL;

	return hTask->environ;
}

/* 
// OSTask_SetEnv - Set environment of the task.
//
// RETURNS: 0: Success, -1: failed.
*/
int OSTask_SetEnv( OSTASK_HANDLE hTask, void* environ )
{
	if (hTask==NULL) return -1;
	
	hTask->environ = environ;

	return 0;
}

/* 
// OSTask_Delay - Delay task for specific time in ms.
//
// RETURNS: 0: Sucess, -1: failed.
*/
int OSTask_Delay( int tminms )
{
	struct timespec delayTime, elaspedTime;

	/* If TMINMS=0, relinquish the processor */
	if (tminms==0) return sched_yield();

	/* Otherwise delay for specific time */
	delayTime.tv_sec  = tminms / 1000;
	delayTime.tv_nsec = ( tminms % 1000 ) * 1000000;

	return nanosleep(&delayTime, &elaspedTime);
}

/* 
// OSTask_Self - Get current task handle.
//
// RETURNS: Current task handle, if NULL means the task is the main task.
*/
OSTASK_HANDLE OSTask_Self( void )
{
	OSTASK_HANDLE hTask = NULL;

	(void)pthread_mutex_lock(&gTaskGlobalInfo.mutex);

	/* Find task ID from task list */
	hTask = (OSTASK_HANDLE)DLL_FIRST(&gTaskGlobalInfo.qTaskHead);
	while (hTask!=NULL) {
		/* For self-task, the IDs is equal */
		if (pthread_equal(hTask->thrid, pthread_self())) break;
		hTask = (OSTASK_HANDLE)DLL_NEXT((DL_NODE*)hTask);
	}
	(void)pthread_mutex_unlock(&gTaskGlobalInfo.mutex);

    return hTask;
}

/* 
// OSTask_GetId - Get ID of the task used by OS.
//
// RETURNS: task id of the task.
*/
void* OSTask_GetId( OSTASK_HANDLE hTask )
{
	return (void*)((hTask==NULL) ? pthread_self() : hTask->thrid);
}

/* End of file */
