#include <stdio.h>
#include <stddef.h>
#include <time.h>

int getCurrentTime(char * flag_name,int flag)
{
	time_t timer;//time_t就是long int 类型
	FILE * fp = NULL;
	struct tm *tblock;
	char * p = NULL;

	fp = fopen("./current_time","a+");
	if(!fp) return -1;
	
	timer = time(NULL);

	tblock = localtime(&timer);

	p =  asctime(tblock);
	printf("Local time is: %s\n", p);
	
	fprintf(fp,"current moudle : %s,current id : %d,current time : %s \n",flag_name,flag,p);
	fclose(fp);
	return 0;
}

#ifdef __TEST__
int main()
{
	int retval = -1;
	
	
	retval = getCurrentTime(__FILE__,__LINE__);
	
}
#endif