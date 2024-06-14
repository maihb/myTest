#include <iostream>
#include <dlfcn.h>
#include <unistd.h>
#include "public.h"

using namespace std;
static string goso = "/home/maihuabao/cn/inteGrationHALService/build/libRouter.so";
typedef void (*_callback)(long long int uid, const char *body, const char *query_string);
typedef void (*_handle)(const char *method, const char *uri, _callback fn);
typedef void (*_start)(const char *addr);
typedef void (*_response)(long long int uid, const char *str);

_handle handle;
_start start;
_response rsp;
void *_hModule;
void load(string name)
{
    _hModule = dlopen(name.c_str(), RTLD_LAZY);
    if (_hModule == NULL)
    {
        printf("dlopen err, dll name=%s\n", name.c_str());
        return;
    }

    handle = (_handle)dlsym(_hModule, "http_handle");
    if (!handle)
    {
        printf("dlsym handle err!\n");
        return;
    }
    start = (_start)dlsym(_hModule, "http_start");
    if (!start)
    {
        printf("dlsym start err!\n");
        return;
    }
    rsp = (_response)dlsym(_hModule, "http_response");
    if (!rsp)
    {
        printf("dlsym rsp err!\n");
        return;
    }
}

void fn(long long int uid, const char *body, const char *query_string)
{
    printf("c.body=%s\n", body);
    printf("c.query_string=%s\n", query_string);
    rsp(uid, "asdfafdafaf\n");
}

void test3()
{
    load(goso);
    handle("GET", "/", fn);
    start(":7777");
    sleep(1000);
}
