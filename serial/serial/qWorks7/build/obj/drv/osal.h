#ifndef _OSAL_H_
#define _OSAL_H_

#include <qWorks.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
//-------------------------------------------------------------------------------
// OSAL INTERFACE
//-------------------------------------------------------------------------------
*/
int Osal_Init( int(*init)(void*), void* arg, void(*cleanup)(void) );
int Osal_Delay( int tminms );

/*
//-------------------------------------------------------------------------------
// OSEVENT INTERFACE
//-------------------------------------------------------------------------------
*/
/* Event wait options */
#define OSEVENT_WAIT_ANY 0x01
#define OSEVENT_WAIT_ALL 0x02
#define OSEVENT_MODE_CLR 0x10

/* Event wait mask */
#define OSEVENT_WAIT_MASK 0x0F

/* Event wait options combination */
#define OSEVENT_WAIT_ANY_CLR (OSEVENT_WAIT_ANY | OSEVENT_MODE_CLR)
#define OSEVENT_WAIT_ALL_CLR (OSEVENT_WAIT_ALL | OSEVENT_MODE_CLR)

/* Event handle */
typedef struct os_event *OSEVENT_HANDLE;

/* function declarations */

OSEVENT_HANDLE OSEvent_Create( int wanted );
int OSEvent_Delete( OSEVENT_HANDLE hEvent );

int OSEvent_Take( OSEVENT_HANDLE hEvent, int mode, int tminms );
int OSEvent_Give( OSEVENT_HANDLE hEvent, int event );
int OSEvent_Clear( OSEVENT_HANDLE hEvent, int event );
int OSEvent_Given( OSEVENT_HANDLE hEvent, int event );

/*
//-------------------------------------------------------------------------------
// MUTEX INTERFACE
//-------------------------------------------------------------------------------
*/
/* Mutex handle */
typedef struct os_mutex *OSMUTEX_HANDLE;

/* function declarations */

OSMUTEX_HANDLE OSMutex_Create( void );
int OSMutex_Delete( OSMUTEX_HANDLE hMutex );

int OSMutex_Take( OSMUTEX_HANDLE hMutex, int tminms );
int OSMutex_Give( OSMUTEX_HANDLE hMutex );

/*
//-------------------------------------------------------------------------------
// SEMAPHORE INTERFACE
//-------------------------------------------------------------------------------
*/
/* Semaphore Handle */
typedef struct os_sem *OSSEM_HANDLE;

/* Semaphore interface */

OSSEM_HANDLE OSSem_Create( int maxCount, int initVal );
int OSSem_Delete( OSSEM_HANDLE hSem );

int OSSem_Take( OSSEM_HANDLE hSem, int tminms );
int OSSem_Give( OSSEM_HANDLE hSem );
int OSSem_Count( OSSEM_HANDLE hSem );

/*
//-------------------------------------------------------------------------------
// MESSAGE QUEUE INTERFACE
//-------------------------------------------------------------------------------
*/
/* Message queue Handle */
typedef struct os_msgq *OSMSGQ_HANDLE;

/* Message send priorities */
#define OSMSG_PRI_NORMAL 0	/* Normal priority message */
#define OSMSG_PRI_URGENT 1	/* Urgent priority message */

/* function declarations */

OSMSGQ_HANDLE OSMsgQ_Create ( int maxMsgCnt, int maxMsgLen );
int OSMsgQ_Delete ( OSMSGQ_HANDLE hmq );

int OSMsgQ_Send( OSMSGQ_HANDLE hmq, char *buffer, int nbytes, int tminms, int priority );
int OSMsgQ_Receive( OSMSGQ_HANDLE hmq, char *buffer, int maxnbytes, int tminms );
int OSMsgQ_Count( OSMSGQ_HANDLE hmq );

/*
//-------------------------------------------------------------------------------
// TASK INTERFACE
//-------------------------------------------------------------------------------
*/
/* Task handle */
typedef struct os_task *OSTASK_HANDLE;

/* Task callback entry */
typedef void* (*OSTASK_FXN)(OSTASK_HANDLE);

/* function declarations */

OSTASK_HANDLE OSTask_Create( int priority, char* stackBase, int stackSize, OSTASK_FXN entry );
int OSTask_Delete( OSTASK_HANDLE hTask );

int OSTask_SetPriority( OSTASK_HANDLE hTask, int priority );
int OSTask_GetPriority( OSTASK_HANDLE hTask );

int OSTask_SetState( OSTASK_HANDLE hTask, int state );
int OSTask_GetState( OSTASK_HANDLE hTask );

int OSTask_SetEnv( OSTASK_HANDLE hTask, void* environ );
void* OSTask_GetEnv( OSTASK_HANDLE hTask );

OSTASK_HANDLE OSTask_Self( void );
void* OSTask_GetId( OSTASK_HANDLE hTask );
int OSTask_Delay( int tminms );

#ifdef __cplusplus
}
#endif

#endif /* _OSAL_H_ */

/* End of file */
