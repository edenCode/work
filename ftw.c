#include <stdio.h>
#include <ftw.h>


//int ftw(const char *dir, int (*fn) (const *file, const struct stat *sb, int flag), int depth)
//int (*fn) (const *file, const struct stat *sb, int flag)
int ftw_callback(const char * file,const struct stat * sb,int flag)
{
    int i = 0;
    char name[1024];

    FILE * fp = fopen("./files.txt","w + a");

    if(!fp) fprintf(stderr,"fopen file failed \n");
    
    printf("ftw_callback %d \n",i++);
    if(flag == FTW_F)
    {
        printf("%s,--file\n",file);
	sprintf(name,"\t %s \n",file);
    }
    else if(flag == FTW_D)
    {
        printf("Dir %s\n",file);
    	sprintf(name,"Dir %s : \n",file);
    }

    fwrite(name,1,sizeof(name),fp);
    fclose(fp);
    return 0; // is_important
}

int main(int argc,char * argv[])
{

	int fd = -1;
	int ret = -1;

	if(argc < 2)
	{
		fprintf(stderr,"%s path\n",argv[0]);
		return -1;
	}

	ret = ftw(argv[1],ftw_callback,10);
	printf("ftw return %d \n",ret);


	return 0;

}
