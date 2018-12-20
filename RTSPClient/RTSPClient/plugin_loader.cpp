#include "plugin_loader.h"
#include <Windows.h>

msdk_so_handle msdk_so_load(const TCHAR *file_name)
{
	if (!file_name) return NULL;
	return (msdk_so_handle) LoadLibrary((LPCTSTR)file_name);
}

msdk_func_pointer msdk_so_get_addr(msdk_so_handle handle, const char *func_name)
{
	if (!handle) return NULL;
	return (msdk_func_pointer)GetProcAddress((HMODULE)handle, /*(LPCSTR)*/func_name);
}

void msdk_so_free(msdk_so_handle handle)
{
	if (!handle) return;
	FreeLibrary((HMODULE)handle);
}

bool AreGuidsEqual(const mfxPluginUID& guid1, const mfxPluginUID& guid2)
{
	for(size_t i = 0; i != sizeof(mfxPluginUID); i++)
	{
		if (guid1.Data[i] != guid2.Data[i])
			return false;
	}
	return true;
}
