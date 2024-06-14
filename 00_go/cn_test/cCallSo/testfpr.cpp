#include <iostream>
#include <dlfcn.h>
#include <unistd.h>
#include "public.h"

using namespace std;
static string goso = "./libFprModule.so";
typedef const char *(*_fprGetInfo)(int useType, int leftRight, int position, int timeout, char *historyFeature, int *errCode);
typedef void (*_fprReadStop)();

_fprGetInfo fprGetInfo_ptr = NULL;
_fprReadStop fprReadStop_ptr = NULL;
static void *handle = NULL;
static void load(const char *name)
{
    if (handle != NULL)
    {
        return;
    }
    handle = dlopen(name, RTLD_LAZY);
    if (!handle)
    {
        errno = ENOENT;
        printf("1xxxxxxxx, err=%s\n", dlerror());
        return;
    }
    fprGetInfo_ptr = (_fprGetInfo)dlsym(handle, "fprGetInfo");
    if (!fprGetInfo_ptr)
    {
        errno = EFAULT;
        printf("2xxxxxxxx, err=%s\n", dlerror());
        return;
    }

    fprReadStop_ptr = (_fprReadStop)dlsym(handle, "fprReadStop");
    if (!fprReadStop_ptr)
    {
        errno = EFAULT;
        printf("3xxxxxxxx, err=%s\n", dlerror());
        return;
    }
}

void test4()
{
    load(goso.c_str());
    if (fprReadStop_ptr)
    {
        fprReadStop_ptr();
    }
    else
    {
        printf("fprReadStop_ptr is null");
        exit(1);
    }
    sleep(1000);
}
