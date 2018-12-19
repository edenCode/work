#include <stdio.h>
#include <string.h>
#include "cJson.h"

typedef unsigned char s8;
int main()
{
	cJSON * pRoot = NULL;
	cJSON * pSub_1 = NULL;
	cJSON * pSub_2 = NULL;
	cJSON * pSub_3 = NULL;

	if((pRoot = cJSON_CreateObject()) == NULL)
    {
        return -1;
    }
	
	cJSON_AddNumberToObject(pRoot,"checkId",1);
	
	#if 0
	pSub_1 = cJSON_AddArrayToObject(pRoot, "gpsState");
    if (pSub_1 == NULL)
    {
        cJSON_Delete(pRoot);
        return -1;
    }
	#endif
	pSub_2 = cJSON_CreateObject();
    if (pSub_2 == NULL)
    {
        cJSON_Delete(pRoot);
        return -1;
    }
	
	cJSON_AddNumberToObject(pSub_2,"state",22);//state->gps_state);
	cJSON_AddStringToObject(pSub_2,"xPosition","11,11,11");

	cJSON_AddStringToObject(pSub_2,"yPosition","11,11,11");
	//cJSON_AddObjectToObject(pRoot,"gpsState");
	cJSON_AddItemToObject(pRoot,"gpsState",pSub_2);
	
	pSub_3 = cJSON_CreateObject();
    if (pSub_3 == NULL)
    {
        cJSON_Delete(pRoot);
        return -1;
    }
	cJSON_AddStringToObject(pSub_3,"test","hello");
	cJSON_AddObjectToObject(pSub_3,"test");
	cJSON_AddObjectToObject(pRoot,"test");
	cJSON_AddRawToObject(pRoot,"eee","test-test-test");
	
	s8 * pJson = cJSON_Print(pRoot);
    if(NULL == pJson)
    {
        cJSON_Delete(pRoot);
        return -1;
    }
	
	
	printf("###############################pJson:%d\n", strlen(pJson));
    printf("%s\n", pJson);
	
	pRoot = cJSON_Parse(pJson);
	
		//cJSON_GetStringValue(pRoot);
		
	printf("str : %s \n",cJSON_GetStringValue(pRoot));
}