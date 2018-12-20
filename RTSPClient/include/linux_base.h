#ifndef LINUX_DEFINE_HH
#define LINUX_DEFINE_HH

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define	LAST_ERROR			errno
#define SOCKET				int32_t
#define INVALID_SOCKET			-1
#define SOCKET_ERROR			-1

#define SOCKET_CLOSE(P)			if(P != -1){close(P);P = -1;}

#endif

