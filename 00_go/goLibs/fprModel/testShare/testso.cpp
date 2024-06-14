#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <fstream>
#include <unistd.h>
#include <iostream>

using namespace std;
bool load_dll(void);
void unload_dll(void);
void *g_handle = NULL;

#define DLLNAME "libFprModule.so"
typedef int(__stdcall *LIVESCAN_Close_)(void);
// const char *fprGetInfo(int useType, int leftRight, int position, const char *historyFeature, int *errCode);
typedef const char *(__stdcall *FprGetInfo_)(int useType, int leftRight, int position, const char *historyFeature, int *errCode);
// int fprModuleInit(const char *pszAppPath);
typedef int(__stdcall *FprModuleInit_)(const char *);
// void fprModuleRelease(void);
typedef void(__stdcall *FprModuleRelease_)(void);

//=============================================================================
LIVESCAN_Close_ LS_Close = NULL;
FprGetInfo_ getInfo = NULL;
FprModuleInit_ moduleInit = NULL;
FprModuleRelease_ fprModuleRelease = NULL;

int main(int argc, char *argv[])
{
    for (int i = 0; i < 200; i++)
    {
        unload_dll();
        bool ok = load_dll();
        // printf("load_dll =%d sizeof(xxx)=%d\n", ok, i);
        int b;
        const char *str = getInfo(1, 1, 2, "ffffff", &b);
        printf("code[%d],GETinfo =[%s]\n", b, str);
        // moduleInit("fssdsfafaf");
        sleep(2);
    }
    return 0;
}

void unload_dll()
{
    if (g_handle != NULL)
    {
        fprModuleRelease();
        dlclose(g_handle);
        g_handle = NULL;
    }
}
bool load_dll()
{
    if (g_handle == NULL)
    {
        char *pszErr;
        string filename = string("/Users/maihuabao/work/goLibs/build/") + DLLNAME;
        if ((g_handle = dlopen(filename.c_str(), RTLD_LAZY)) == NULL) // RTLD_NOW
        {
            filename = string("./amd64/") + DLLNAME;
            g_handle = dlopen(filename.c_str(), RTLD_LAZY);
            if (g_handle == NULL)
            {
                printf("Failed load library: %s\n", DLLNAME);
                printf("Error is: %s\n", dlerror());
                return false;
            }
        }
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            g_handle = NULL;
            return false;
        }
        getInfo = (FprGetInfo_)dlsym(g_handle, "fprGetInfo");
        moduleInit = (FprModuleInit_)dlsym(g_handle, "fprModuleInit");
        fprModuleRelease = (FprModuleRelease_)dlsym(g_handle, "fprModuleRelease");

        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            dlclose(g_handle);
            g_handle = NULL;
            return false;
        }
    }

    return g_handle != NULL;
}
