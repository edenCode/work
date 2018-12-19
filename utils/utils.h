#include "stdio.h"

int getCurrentTime(char * flag_name,int flag);

int  UTF_8ToGB2312(char*pOut, char *pInput, int pLen);
int UTF_8ToUnicode(char* pOutput, char *pInput);
void UnicodeToGB2312(char*pOut, char *pInput);