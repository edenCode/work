#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
int main()
{ 
	char * ptr = NULL;
	char * endptr = NULL;
	char str[] = "test,hhh,ttttt,nnnn mmm eeeee";
	char num[] = "123456789aaaAaabbbAAA";
	char  p[] = "a,A,h,AAAA,BBBB,CCCC,1111,22222,1111222";
//	printf("%s\n",wcschr(str,"hhh")); // 字符g扩展，wchar_t 表示更多的字符，且超过了8位。
	printf("%s\n",strstr(str,"hhh"));
	printf("%s\n",strchr(str,','));
	printf("%s\n",strchr(str,'e'));
	printf("%ld  endptr %s\n",strtoll(num,&endptr,10),endptr);	
	printf("%d  endptr %s\n",strtol(num,&endptr,10),endptr);
	
	printf("p %s \n",p);
//	ptr = strtok(p,",");
//	printf("ptr %s \n",ptr);	
//	ptr = strtok(ptr,",");
//	printf("ptr %s \n",ptr);

	ptr = p;
	ptr = strtok(p,",");
	printf("ptr %s \n",ptr);	
	
	while(ptr)
	{	
	  ptr = strtok(NULL,",");
	  printf("ptr %s \n",ptr);	
	
	}	
	return 0;
}
