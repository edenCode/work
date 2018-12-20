#include "strDup.hh"
#include "string.h"

char* strDup(char const* str)
{
	if (str == NULL) {
		return NULL;
	}
	size_t len = strlen(str) + 1;
	char* copy = new char[len];

	if (copy != NULL) {
		memcpy(copy, str, len);
	}
	return copy;
}

char* strDupSize(char const* str) 
{
	if (str == NULL) {
		return NULL;
	}
	size_t len = strlen(str) + 1;
	char* copy = new char[len];
	return copy;
}

