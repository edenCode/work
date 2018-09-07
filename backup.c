#ifdef RECOG_PLATE_TEST // gongcm


int NV12ToBGR( unsigned char* yuv, unsigned char* bgr, int w, int h )
{
	int i, j, m;
	int y, u, v;
 unsigned char *yp, *cp;
	
	printf("\n\n w ； %d h : %d \n\n",w,h);
    if (w<1 || h<1 || yuv==NULL || bgr==NULL)
        return 0;
    yp = yuv; // Y plane, Size=WxH
    cp = yuv + w*h; // UV interleaved plane, Size=(W/2)x(H/2)
    for (j=0; j<h; j+=2) {
    	for (i=0; i<w; i+=2) {
			u = *cp++;
			v = *cp++;
			m = w*j+i; // (Y00, U00, V00) --> P00
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m ++;      // (Y01, U00, V00) --> P01
			y = yp[m];
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m += w;    // (Y11, U00, V00) --> P11
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
			m --;      // (Y10, U00, V00) --> P10
			y = yp[m]; 
			bgr[m*3+0] = CLAMP(YUV_TO_B(y, u, v), 0, 255);
			bgr[m*3+1] = CLAMP(YUV_TO_G(y, u, v), 0, 255);
			bgr[m*3+2] = CLAMP(YUV_TO_R(y, u, v), 0, 255);
    	}
    }
	
    return 1;
}



typedef  struct data_{
	int w;
	int h;
	int size;
	int type;
	//unsigned char * buffer;
}Data;

//#define SAVEIMG 1   // 保存图片ROI

typedef struct TagInfo
{
	char color[48];
	char type[48];
	char logo[48];
	char plate[LPR_PLATE_STR_LEN];
}Info;

static void recongByTcp(void * param)
{
	int retval = -1;
	int sockfd = -1;
	int i = 0;
	int j = 0;
	int share = 1;
	struct sockaddr_in servAddr;
	int size;

	int recvBufsize = 0;

	
	Data data;
	unsigned char * buffer = NULL;
	struct sockaddr_in clntAddr;
	socklen_t clntAddrLen = sizeof(clntAddr);
	int newSocket = -1;
	char log[100] = {0};
	unsigned char * temp = NULL;
	int tatol = 0;

	int sn;
	fd_set fdr;
	struct timeval tv;

	VSDRect recogarea;
	LPROutput LPR_out;
	unsigned int Identify_time = 0;

	//unsigned char * yuv = NULL;
	unsigned char *y_data = NULL;
	unsigned char *uv_data = NULL;

	Rect rect;
	Classifier_Result * Result = NULL;
	unsigned char * yuv = (unsigned char *)malloc(2048 * 1536 * 3);

	Info info;
	char ** p = NULL;
	
//#ifdef SAVEIMG

	FILE * fp = NULL;
	char path[48] ={0};
//#endif		

//	data.buffer = buffer;

	if(!yuv)
{printf("yuv malloc failed \n");}
	//memset(&buffer,0,sizeof(buffer));
	
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);;
	if(sockfd < 0)
	{
		printf("socket create failed  %d\n",__LINE__);

		
	}


	printf(" recongnized image by tcp ... ,sockfd %d \n ",sockfd);
	int flags = fcntl(sockfd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, flags);


	if(sockfd > 0)
	{
			
		//setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &share, sizeof(int));

	
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family             = AF_INET;
		servAddr.sin_addr.s_addr        = htonl(INADDR_ANY);   //Any IP address available locally
		servAddr.sin_port               = htons(39999);

 		if((retval = bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0))
 		{
 			perror("bind socket : ");
 			printf("error bind socket \n");
 		}

		if((retval = listen(sockfd,5) ) < 0)
		{
			perror("Listen socket : ");
			printf("error listen sockfd \n");
		}

		


			
		while(1)
		{

			if((newSocket = accept(sockfd, (struct sockaddr *)&clntAddr, &clntAddrLen)) < 0)
			{
				//printf(" error client \n");
				continue;
			}


			printf("client ip:%s, newSocket = %d.\n", inet_ntoa(clntAddr.sin_addr), newSocket);

			
			
			while(1)
			{

			
				retval = recv(newSocket,&data,sizeof(data),0);
				if(retval <= 0)
				{
					printf("error recv bytes ,retval  %d \n",retval);
					close(newSocket);
					break;
				}
							
						if(retval == sizeof(data))
						{
						
							//memcpy(&data,buffer,sizeof(data));
							printf(" recv %d bytes,w %d,h %d,type %d,size %d \n",retval,data.w,data.h,data.type,data.size);
						

							
							recogarea.x = data.w / 4;
							recogarea.y = data.h / 4;
							recogarea.width = data.w - data.w / 4 - 100;
							recogarea.height = data.h - data.h / 4 - 100;

							
							size = data.size;
							if(buffer) free(buffer);
							buffer = (unsigned char *) malloc(size);

							temp = buffer;
							j = 0;
							tatol = 0;
							retval = 0;
							while(size > 0)
							{
							
								//temp += size;
							
								retval = recv(newSocket,temp,size,0);
								if(retval <= 0)
								{
									printf(" wait %d  recv ... \n",retval);
									//close(newSocket);
									break;
								}
							
								size -= retval;
								temp += retval;
								tatol += retval;
							
							}

							printf("buffer size %d,imgsize %d bytes,recv tatla size %d bytes ... \n",(2048 * 1536 * 3),data.size,tatol);
							if(tatol != data.size) // 接收数据校验
							{
								printf("recv over,data err,close socket\n");
								//FD_CLR(newSocket,&fdr);
								close(newSocket);
								break;
							}
						#ifdef SAVEIMG
							sprintf(path,"%dx%d_img.yuv",data.w,data.h);
							fp = fopen(path,"w+");
							if(fp)
							{
								retval = fwrite(buffer,1,tatol,fp);
								printf("save %d bytes  save %s file ... \n",retval,path);

								fflush(fp);
								fclose(fp);
							}
						#endif
							y_data = buffer;
							uv_data = buffer + data.w * data.h ;
						
							retval = recog_plate(y_data,uv_data,data.w,data.h,&recogarea,&LPR_out,0,&Identify_time);
							if(retval < 0)
							{
								printf(" recong_plate Faield \n");
								close(newSocket);
								break;
							}
						
							printf(">>>>>>>>>>>>>>>>> plate : 	%s x[%d]y[%d]w[%d]h[%d] <<<<<<<<<<<<<<<\n",LPR_out.wszRec, \
							LPR_out.rectPlate.x,
							LPR_out.rectPlate.y,
							LPR_out.rectPlate.width,
							LPR_out.rectPlate.height);
							
							rect.x = LPR_out.rectPlate.x;
							rect.y = LPR_out.rectPlate.y;
							rect.width = LPR_out.rectPlate.width;
							rect.height = LPR_out.rectPlate.height;

							if(rect.width == 0 && rect.height == 0) // 识别不成，保存图片
							{
								sprintf(path,"%dx%d_img.yuv",data.w,data.h);
								fp = fopen(path,"w+");
								if(fp)
								{
									retval = fwrite(buffer,1,tatol,fp);
									printf("save %d bytes  save %s file ... \n",retval,path);

									fflush(fp);
									fclose(fp);
								}

								memset(&data,0,sizeof(data)); // 清空上次接收数据
								close(newSocket);
								break;//重新按协议发一遍数据
							}
					
							Yuv_ROI(yuv,yuv + data.w * data.h, &rect, y_data, uv_data, data.w, data.h );
						#ifdef SAVEIMG
							printf("ROi pointer width %d,height %d ... \n",rect.width,rect.height);
							sprintf(path,"%dx%d_img_ROI.yuv",rect.width,rect.height);
							fp = fopen(path,"w+");
							if(fp)
							{
								retval = fwrite(yuv,1,rect.width * rect.height,fp);
								retval = fwrite(yuv + data.w * data.h,1,rect.width * rect.height / 2,fp);
								printf("save %d bytes  save %s file ... \n",retval,path);

								fflush(fp);
								fclose(fp);
							}
						#endif
							retval = recog_carInfo(yuv,yuv + data.w * data.h,rect.width,rect.height,&Result);
					#if 0
							printf(">>>>>>>>>>>>>>>  %s  %s  %s  \n",carcolor[Result[0].classify],cartype[Result[1].classify],carlogo[Result[2].classify]);
							for(i = 0; i < 3; i ++) // 发送识别结果
							{
								retval = send(newSocket,&Result[i],sizeof(Classifier_Result),0);
								printf("classify %d ,confid %d \n",Result[i].classify,Result[i].confid);
								printf("send  %d bytes ... \n",retval);
							}
					#endif	
							
							p = &carcolor[0];
							strncpy(info.color,p[Result[0].classify],strlen(p + Result[0].classify)+1);
							p = &cartype[0];
							strncpy(info.type,p[Result[1].classify],strlen(p + Result[1].classify)+1);
							p = &carlogo[0];
							strncpy(info.logo,p[Result[2].classify],strlen(p + Result[2].classify)+1);
							strcpy(info.plate,LPR_out.wszRec);

							sleep(5);
							retval  = send(newSocket,&info,sizeof(info),0);
							if(retval <= 0)
							{
								printf("info send failed ,%d\n",retval);
							}
							printf(" send %d bytes,color %s ,type %s ,logo %s , plate %s \n",retval,info.color,info.type,info.logo,info.plate);
			
						}
					
					
			}

			close(newSocket);
			usleep(100000);
		}
		close(sockfd);
	}
}

#if 1
#define W 2048
#define H 1536
#define L (((((W+3)*3)/4)*4)*H)

#define TEST_YUV_WIDTH			((W / 4) * 4)
#define TEST_YUV_HEIGHT			((H /4 ) * 4)


//#define platefile "/opt/sys/2048x1536_0001.yuvsp"

//#define Graph "/opt/ipnc/test/graph"

#define BGRI "/opt/ipnc/test/"


static void recogTestImg(void * param)
{

	printf("\n\n\n######################## Begin recogTestImg\n\n");
	
	int i = 16,j = 0;
	VSDRect recogarea;
	LPROutput LPR_out;
	struct timeval tv_start;
	struct timeval tv_end;
	struct tm * tmv;
	unsigned int temp_time = 0;
	unsigned int Identify_time = 0;

	unsigned char * yuv = NULL;
	unsigned char *y_data = NULL;
	unsigned char *uv_data = NULL;
	unsigned char *pData = (unsigned char *)malloc(TEST_YUV_WIDTH * TEST_YUV_HEIGHT * sizeof(char)*3/2);
	unsigned char * bgr  = (unsigned char *)malloc(TEST_YUV_WIDTH * TEST_YUV_HEIGHT * sizeof(char)*3/2);
	unsigned char * yp = (unsigned char *)malloc(TEST_YUV_WIDTH * TEST_YUV_HEIGHT * sizeof(char)*3/2);
	char name[256];
	Rect rect; 
	
	FILE * fp_w = NULL;
	
	if (pData == NULL || bgr == NULL)
	{
		printf("buf malloc failed \n");
		return ;
	}
	
	//fp_w = fopen("./test.data","a+");
	y_data = pData;
	uv_data = pData  + TEST_YUV_WIDTH * TEST_YUV_HEIGHT;
	
	//OSAL_HANDLE m_TypeHandle=OSAL_NULL;
	SDKErrCode e_Code;
	DetectRect prect;
	Classifier_Result*Result = NULL;
	
	//e_Code = MV_VehicleTaskCreate(&m_TypeHandle,224,224,Graph);//计算棒初始化；
	//printf("MV_VehicleTaskCreate handle %p  e_Code : %d \n",m_TypeHandle,e_Code);
	while(1)
	{
		if(i == 0) i = 16;
		sprintf(name,"/sdcard/2048x1536/2048x1536_%0.4d.yuvsp",i);
		i --;
		printf(" >>>>>>>>>>>>  file open %s  \n ",name);
		FILE *fp = fopen(name, "r");

		//printf(">>>>>>>>>>>>>> filename : %s \n",platefile);
	if (fp)
	{	
			memset(&recogarea, 0, sizeof(recogarea));
			memset(&LPR_out, 0, sizeof(LPR_out));
			fread(pData,TEST_YUV_WIDTH * TEST_YUV_HEIGHT * sizeof(char)*3/2,1,fp);		
			fread(y_data, TEST_YUV_WIDTH * TEST_YUV_HEIGHT, 1, fp);
			fread(uv_data, TEST_YUV_WIDTH * TEST_YUV_HEIGHT / 2, 1, fp);
			fclose(fp);

			// recogarea set
			recogarea.x = TEST_YUV_WIDTH/4;
			recogarea.y = TEST_YUV_HEIGHT/4;
			recogarea.width = TEST_YUV_WIDTH - TEST_YUV_WIDTH/4-100;
			recogarea.height = TEST_YUV_HEIGHT -TEST_YUV_HEIGHT/4-100;
			printf("recongnize area x[%d] y[%d] width %d height %d \n",recogarea.x,recogarea.y,recogarea.width,recogarea.height);
	
			int ret = recog_plate(y_data,uv_data,TEST_YUV_WIDTH,TEST_YUV_HEIGHT,&recogarea,&LPR_out,0,&Identify_time);
			printf(">>>>>>>>>>>>>>>>> plate : %s x[%d]y[%d]w[%d]h[%d] <<<<<<<<<<<<<<<\n",LPR_out.wszRec, \
					LPR_out.rectPlate.x,
					LPR_out.rectPlate.y,
					LPR_out.rectPlate.width,
					LPR_out.rectPlate.height);
					
			rect.x = LPR_out.rectPlate.x;
			rect.y = LPR_out.rectPlate.y;
			rect.width = LPR_out.rectPlate.width;
			rect.height = LPR_out.rectPlate.height;
			
			Yuv_ROI(bgr,yp,(Rect *)&rect,y_data,uv_data,TEST_YUV_WIDTH,TEST_YUV_HEIGHT);
			printf(">>>>>>>>>>>>>>>>> cut yuv roi x[%d]y[%d]w[%d]h[%d] <<<<<<<<<<<<<<<\n", \
					rect.x,
					rect.y,
					rect.width,
					rect.height);
			sprintf(name,"/opt/sys/%d_%d_after.yuv",rect.width,rect.height);
			printf(">>>>>>> ROi filename : %s \n",name);
			fp_w = fopen(name,"w+");
			if(!fp_w){ printf("error : open %s \n",name);}
				
			fwrite(bgr,rect.width * rect.height,1,fp_w);
			fwrite(yp,rect.width * rect.height/2,1,fp_w);
			fclose(fp_w);
			//fclose(fp);
		
#if 0	

		// 颜色转换
			gettimeofday(&tv_start,0);
			NV12ToBGR(pData,bgr,TEST_YUV_WIDTH,TEST_YUV_HEIGHT);
		gettimeofday(&tv_end,0);
		temp_time = ((tv_end.tv_sec - tv_start.tv_sec) * 1000 + tv_end.tv_usec / 1000) - tv_start.tv_usec / 1000;
		printf("NV12ToBGR temp time in recong img temp_time=%d ms\n",temp_time);
		printf("NV12 transfer to BGR successful img  width : %d height : %d ,bgr ：%p, size : %d bytes \n",TEST_YUV_WIDTH,TEST_YUV_HEIGHT,bgr,W * H * 3);

		gettimeofday(&tv_start,0);
		pData = get_nerv_yuvaddr();
		memcpy(yuv,bgr,rect.width * rect.height);
		memcpy(yuv,yp,rect.width * rect.height / 2);
		NV12_2_BGR(rect.width,rect.height);
		gettimeofday(&tv_end,0);
		temp_time = ((tv_end.tv_sec - tv_start.tv_sec) * 1000 + tv_end.tv_usec / 1000) - tv_start.tv_usec / 1000;
		printf("NV12ToBGR temp time in recong img temp_time=%d ms\n",temp_time);

		if(m_TypeHandle) printf(" MV_VehicleRecog go start	..... \n");

		gettimeofday(&tv_start,0);
		
		e_Code = MV_VehicleRecog(m_TypeHandle,pData,rect.width,rect.height,&Result,callback_BGR_proc);
		gettimeofday(&tv_end,0);
		temp_time = ((tv_end.tv_sec - tv_start.tv_sec) * 1000 + tv_end.tv_usec / 1000) - tv_start.tv_usec / 1000;
		printf("MV_VehicleClassify temp time in recong img temp_time=%d ms\n",temp_time);
		printf("MV_VehicleClassify err_code : %d \n",e_Code);
			
			for(j = 0; j < 3; j ++)
			{
				printf("num = %d classify : %d confid : %d \n",j,Result[j].classify,Result[j].confid);
			}
			
			printf(">>>>>>>>>>>>>>> %s %s %s  \n",carcolor[Result[0].classify],cartype[Result[1].classify],carlogo[Result[2].classify]);
#endif

				
	}
	

	sleep(10);
	}

	free(pData);
	free(bgr);
	y_data = NULL;
	uv_data = NULL;
	
	printf("######################## recogTestImg thread exit\n");
}

#else

static void recogTestImg(void * param)
{
	FILE * fp = NULL,*fp_w = NULL;
	int j = 0;
	char file[48];
	unsigned char * bgr = NULL;
	
	OSAL_HANDLE m_TypeHandle=OSAL_NULL;
	SDKErrCode e_Code;
	DetectRect prect;
	Classifier_Result*Result = NULL;
	
	struct timeval tv_start;
	struct timeval tv_end;
	struct tm * tmv;
	unsigned int temp_time = 0;
		
		sprintf(file,"%s/%dx%d_test.bgr",BGRI,W,L);
		printf("\n\n Image width : %d height : %d  imgLen : %d filename : %s \n\n",W,H,L,file);
	
	unsigned char * buffer = (unsigned char *)malloc(L);
	bgr =(unsigned char *) malloc(L);
	
	if(!buffer || !bgr)
	{
		printf("malloc buffer failed \n");
		return;	
	}
	

	
	e_Code = MV_VehicleTaskCreate(&m_TypeHandle,224,224,Graph);//计算棒初始化；
		printf("MV_VehicleTaskCreate handle %p  e_Code : %d \n",m_TypeHandle,e_Code);
	
	fp = fopen(platefile,"r");
	fp_w = fopen(file,"w+");
	
	if(fp)
	{
		fread(buffer,L,1,fp);
		fclose(fp);
		
		if(fp_w)
		{
			fwrite(buffer,L,1,fp_w);
			printf("Save file %s \n",file);
			fflush(fp_w);
			fclose(fp_w);
		}
		
		prect.x = 0;//LPR_out.rectPlate.x + recogarea.x;
		prect.y = 0;//LPR_out.rectPlate.y + recogarea.y;
		prect.width = W; //(LPR_out.rectPlate.width * 4) /4;
		prect.height = H;//(LPR_out.rectPlate.height *4)/4;
		
		// To BGR
		printf("NV12 to BGR ............\n");
		NV12ToBGR(buffer,bgr,W,H);
		
		//e_Code = MV_VehicleRecog(m_TypeHandle,bgr,W,H,&Result);
		e_Code = MV_VehicleClassify(m_TypeHandle,bgr,W,H,&prect,&Result);
		printf("MV_VehicleClassify e_Code : %d \n",e_Code);
			
		for(j = 0; j < 3; j ++)
		{
				printf("num = %d classify : %d confid : %d \n",j,Result[j].classify,Result[j].confid);
		}
	}
	
}
#endif
#endif // gongcm end