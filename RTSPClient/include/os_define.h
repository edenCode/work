#ifndef OS_DEFINE_HH
#define OS_DEFINE_HH

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include <string>
#include <string.h>

#define os_min(a, b)						(((a) < (b))? (a) : (b))
#define os_max(a, b)						(((a) < (b))? (b) : (a))

#define SAFE_FREE(P)						if (P){free(P); P = NULL;}
#define SAFE_DELETE(P)                      if (P){delete P; P = NULL;}


#define MAX_BUFFER_1KSZ						0x400	 //1k
#define MAX_BUFFER_4KSZ						0x1000	 //4k
#define MAX_BUFFER_8KSZ						0x2000	 //8k
#define MAX_BUFFER_16KSZ					0x4000	 //16k
#define MAX_BUFFER_32KSZ					0x8000	 //32k
#define MAX_BUFFER_64KSZ					0x10000	 //64k
#define MAX_BUFFER_128KSZ					0x20000	 //128k
#define MAX_BUFFER_256KSZ					0x40000	 //256k
#define MAX_BUFFER_512KSZ					0x80000	 //512k
#define MAX_BUFFER_1MSIZE					0x100000 //1M
#define MAX_BUFFER_2MSIZE					0x200000 //2M
#define MAX_BUFFER_4MSIZE					0x400000 //4M


#ifdef WIN32
#include "win32_base.h"
#pragma warning(disable:4995)
#else
#include "linux_base.h"
#endif

#define OS_DYNAMIC_DECLARE(class_name)				\
public:												\
	static class_name* instance_;					\
	static class_name* instance();					\
	static void close();

#define OS_DYNAMIC_IMP(class_name)					\
	class_name* class_name::instance_ = NULL;		\
	class_name * class_name::instance(){			\
		if(NULL == instance_){						\
			instance_ = new class_name() ;			\
		}											\
	return instance_;								\
	}												\
	void class_name::close(){						\
		if(NULL != instance_){						\
			delete instance_;						\
			instance_ = NULL;						\
		}											\
	}


#endif
