#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "ringbuffer.h"

#define MIN(a,b) ((a) < (b))? (a) : (b)
#define MAX(a,b) ((a) > (b))? (a) : (b)



int RingBufferInit(RingBuffer * buffer,int size)
{
    int iret = -1;

	if(!buffer)
	{
		return iret;
	}

	memset(buffer,0,sizeof(RingBuffer));

	if(size < 0) return -2;

	buffer->size = size;
	buffer->pw = buffer->pr = 0;
	buffer->left = size;

	//buffer->is_full = 0;
	pthread_mutex_init(&(buffer->mutex),0);

	buffer->buffer = (char *)malloc(size);
	if(!(buffer->buffer))
	{
		return -3;
	}

	return 0;
}


int isBufferFull(RingBuffer * buffer)
{
	return (buffer->left == 0) ? 1 : 0;
}

int isRingBufferEmpty(RingBuffer * buffer)
{
	return (buffer->left == buffer->size)? 1 : 0;
}

int WriteRingBuffer(RingBuffer * buffer,char * data,int size)
{
	int iret = size;
	int left = 0;
	pthread_mutex_lock(&(buffer->mutex));

	if(isBufferFull(buffer))
	{
		pthread_mutex_unlock(&(buffer->mutex));
		return -2;

	}

	if(buffer->left > size)
	{
		buffer->left -= size;
		left = MAX(buffer->pw + size,buffer->size);
		if(left == buffer->size)
		{
			memcpy(buffer->buffer + buffer->pw,data,size);
			buffer->pw += size;

		}
		else
		{
			left = size - (buffer->size - buffer->pw);
			memcpy(buffer->buffer+ buffer->pw,data,left);
			buffer->pw = (buffer->pw + size) % buffer->size;
			memcpy(buffer->buffer,data + (size - left),buffer->pw);
		}
	}
	else
		iret = -1; // Not space to save,waiting to read

	pthread_mutex_unlock(&(buffer->mutex));

	return iret;
}

int ReadRingBuffer(RingBuffer * buffer,char * data,int size)
{
	int iret = size;
	int copysize = 0;
	pthread_mutex_lock(&(buffer->mutex));

	if(buffer->left < 0 || size > buffer->size || buffer->left > buffer->size)
	{
		pthread_mutex_unlock(&(buffer->mutex));
		return -2;
	}

	if(buffer->left != 0)
	{
		copysize = buffer->size - buffer->left;

		if(size > copysize)
		{
			pthread_mutex_unlock(&(buffer->mutex));
			return -3;
		}

		buffer->left += size;

		if(buffer->pw > buffer->pr)
		{
			memcpy(data,buffer->buffer + buffer->pr,size);
			buffer->pr += size;
		}
		else
		{
			copysize = size - (buffer->size - buffer->pr);
			memcpy(data,buffer->buffer + buffer->pr,copysize);
			buffer->pr = (buffer->pr + size) % buffer->size;
			memcpy(data + copysize,buffer->buffer,buffer->pr);
		}
	}
	else
		iret = -1;
	pthread_mutex_unlock(&(buffer->mutex));
	return iret;
}
