#include <Windows.h>
#include <stdio.h>

#define user32_module 0x63C84283u

void* GetModule(unsigned int hash);
PROC GetExportAddressSub(void *module, const char *name);
PROC GetExportAddress(void *module, const char *name);