#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statfs.h> 

typedef int s32;

 int getSDstate(char * mount_point)
{
	struct statfs s;
	float total,free;
	if(0 != statfs(mount_point,&s))
	{
		printf("error \n");
		return 0;
	}

	
    	total = (float)(  (s.f_bsize / 1024 ) * s.f_blocks );
    	free = (float) ( ( s.f_bsize / 1024 ) * s.f_bavail );
    	printf("total %f kb,free %f kb, %f\n",total,free,free / total);
	printf("\n\n total %d GB,free %d GB\n\n",(int)((float)((float)(total / 1024) / 1024)),(int)(free / 1024 / 1024));
	
	return (int)((free / total) * 100);
}

int main()
{
 	char dev[255];
 	getcwd(dev,sizeof(dev)-1);
  	printf("path %s,sd : %d \n",dev,getSDstate("/sdcard"));
}
