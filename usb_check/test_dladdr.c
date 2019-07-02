       #define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
//#include <link.h>
#if 0
/*
*
*struct {
*const char *dli_fname;
*void *dli_fbase;
*const char *dli_sname;
*void *dli_saddr;
*size_t dli_size; /* ELF only */
*int dli_bind; /* ELF only */
*int dli_type;
*};
*
*
**/
#endif

int main()
{
	Dl_info  info;
	dladdr(dladdr, &info);
	printf("DIR :  %s \n",info.dli_fname);
	
		dladdr(printf, &info);
	printf("DIR : printf %s \n",info.dli_fname);
	return 0;
}