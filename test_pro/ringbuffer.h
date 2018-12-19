/*
 * ringbuffer.h
 *
 *  Created on: 2018Äê11ÔÂ1ÈÕ
 *      Author: gongcm
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_


typedef struct ringbuffer{
	int pw;
	int pr;
	int size;
	int is_full;
	int left;
	char * buffer;
	pthread_mutex_t mutex;
}RingBuffer;

typedef enum buffer_status{
	_BUFFER_ERR_e = -1,
	_BUFFER_NO_SPACE_e = -2,
	_BUFFER_INIT_FAILE_e = -3,
	_BUFFER_NO_DATA_e = -4
}Buffer_status;

int RingBufferInit(RingBuffer * buffer,int size);
int ReadRingBuffer(RingBuffer * buffer,char * data,int size);
int WriteRingBuffer(RingBuffer * buffer,char * data,int size);


#endif /* RINGBUFFER_H_ */
