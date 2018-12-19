#include <stdio.h>
#include <ftw.h> \\ thread safe

/*
dr-xr-xr-x    2 root     root            0 Oct 23 03:45 attr
-r--------    1 root     root            0 Oct 23 03:45 auxv
--w-------    1 root     root            0 Oct 23 03:45 clear_refs
-r--r--r--    1 root     root            0 Oct 23 03:45 cmdline
-rw-r--r--    1 root     root            0 Oct 23 03:45 comm
-rw-r--r--    1 root     root            0 Oct 23 03:45 coredump_filter
lrwxrwxrwx    1 root     root            0 Oct 23 03:45 cwd -> /opt/ipnc
-r--------    1 root     root            0 Oct 23 03:45 environ
lrwxrwxrwx    1 root     root            0 Oct 23 03:45 exe -> /opt/ipnc/Bayonet
dr-x------    2 root     root            0 Oct 23 03:45 fd
dr-x------    2 root     root            0 Oct 23 03:45 fdinfo
-r--r--r--    1 root     root            0 Oct 23 03:45 limits
-r--r--r--    1 root     root            0 Oct 23 03:45 maps
-rw-------    1 root     root            0 Oct 23 03:45 mem
-r--r--r--    1 root     root            0 Oct 23 03:45 mountinfo
-r--r--r--    1 root     root            0 Oct 23 03:45 mounts
-r--------    1 root     root            0 Oct 23 03:45 mountstats
dr-xr-xr-x    6 root     root            0 Oct 23 03:45 net
-rw-r--r--    1 root     root            0 Oct 23 03:45 oom_adj
-r--r--r--    1 root     root            0 Oct 23 03:45 oom_score
-rw-r--r--    1 root     root            0 Oct 23 03:45 oom_score_adj
-r--------    1 root     root            0 Oct 23 03:45 pagemap
-r--------    1 root     root            0 Oct 23 03:45 personality
lrwxrwxrwx    1 root     root            0 Oct 23 03:45 root -> /
-r--r--r--    1 root     root            0 Oct 23 03:45 smaps
-r--r--r--    1 root     root            0 Oct 23 03:45 stat
-r--r--r--    1 root     root            0 Oct 23 03:45 statm
-r--r--r--    1 root     root            0 Oct 23 03:45 status
dr-xr-xr-x   23 root     root            0 Oct 23 03:45 task
-r--r--r--    1 root     root            0 Oct 23 03:45 wchan
*/
typedef enum{
	TASK_OK_E = 0;
	TASK_FAILED_E = -1;
}Task_Status;

typedef enum{
	
}Task_Attrs;
static int getTaskProc(int attr)
{
	pid_t pid;
	char dir[48];
	FILE * fp = NULL;
	
	pid = getpid();
	printf("Current Task pid %d \d",pid);
	sprintf(dir,"/proc/%d/",pid,);
	
	if(access(dir,F_OK) != 0)
	{
		return TASK_FAILED_E;
	}
	
	fp = fopen(dir,"r");
	if(!fp)
	{
		perror("fopen :");
		return TASK_FAILED_E;
	}
}
int getTaskStackSize()
{

}