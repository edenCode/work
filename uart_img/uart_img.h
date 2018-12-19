//#define DEV_NAME "/dev/ttyUSB0"
//#define DEV_NAME "/dev/ttyS0"
//#define DEV_NAME "/dev/ttyS1"
//#define DEV_NAME "/dev/ttyS2"
#define DEV_NAME "/dev/ttyS"

#define MD5_SIZE 32
#define P_NUM 3
#define MAGIC 0xFFFFFFFF

//#define __MD5__
int uart_fd_ = -1;
typedef enum{
	MAGICHead =0xFFFFFFFF,
}Magic_head;


typedef struct Header{
	char magic[4]; // start code,eg : 0xFFFFFFFF
	char name[128];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
#ifdef __MD5__
	char MD5[MD5_SIZE];
#endif
}Header;

