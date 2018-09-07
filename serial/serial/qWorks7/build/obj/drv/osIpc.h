#ifndef _OSIPC_H_
#define _OSIPC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
// System message data 
*/
typedef struct IPC_MSG_DATA
{
	long dstid;  /* Dst of message type */
	long srcid;  /* Src of message type */
	int	 cmdid;  /* Command ID */
	int	 shmid;  /* Share memory ID */
	int	 offset; /* Offset to begin of share memory */
	int	 length; /* Data length */
} IPC_MSG_DATA;

/* 
// Share memory 
*/ 
int OSIpc_ShmCreate( int key, int len );
int OSIpc_ShmDelete( int shmid );
int OSIpc_ShmRead( int shmid, int offset, void *buf, int len );
int OSIpc_ShmWrite( int shmid, int offset, void *buf, int len );

/* 
// Message queue 
*/ 
int OSIpc_MqCreate( int key );
int OSIpc_MqDelete( int qid );
int OSIpc_MqSend( int qid, void *pdata, int size, int bWaited );
int OSIpc_MqReceive( int qid, int type, void *pdata, int size, int bWaited );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
