#include <iostream>
#include <unistd.h>
#include "public.h"
#include "IDCardUnite.h"

#define VERSION "1.2.1.6"

using namespace std;
const char *key = "kk1";
string goso = "../goSo/build/libf10.so";
string cso = "../cSo/C.so";

void test1(string name)
{
    int i = 0;
    cout << "-------------welcome to use inteGrationHAL app---------" << endl;
    IDCardUnite abc;
    abc.Init("./", name.c_str());
    printf("xxxxxxx  key[%s] value[%s]\n", key, abc.getInfo(key));
    i = abc.setInfo(key, "fffffffffffff");
    printf("aaaaa i = %d\n", i);
    printf("key[%s] value[%s]\n", key, abc.getInfo(key));
    abc.UnInit();

    IDCardUnite ddd;
    ddd.Init("./", name.c_str());
    printf("ddddd  key[%s] value[%s]\n", key, ddd.getInfo(key));
    i = ddd.setInfo(key, "ddddd");
    printf("dddd  i = %d\n", i);
    printf("dddddddd = key[%s] value[%s]\n", key, ddd.getInfo(key));
    i = abc.setInfo(key, "sdfadfadfadfadf沙发fad");
    printf("abc    ==  key[%s] value[%s]\n", key, abc.getInfo(key));
}

void test2(string name)
{
    IDCardUnite abc;
    abc.Init("./", name.c_str());
    printf("key[%s] value[%s]\n", key, abc.getInfo(key));

    abc.setInfo(key, "fffffffffffff");
    printf("key[%s] value[%s]\n", key, abc.getInfo(key));
    abc.UnInit();

    abc.setInfo(key, "after unLoad");
    printf("key[%s] value[%s]\n", key, abc.getInfo(key));
    char k[10];
    for (int i = 1; i < 100; i++)
    {
        sprintf(k, "key[%d]", i);
        abc.setInfo(k, "after a");
        printf("key[%s] value[%s]\n", k, abc.getInfo(k));
        sleep(5);
    }
}

int main(void)
{
    // test4();
    test3();
    // test2(goso);
    //  sleep(3);
    //  test2(cso);

    // test1(cso);
    sleep(3);
    return 0;
}
