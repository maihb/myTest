#include <iostream>
#include <string>
#include <cctype>
#include "file.h"

using namespace std;

struct flexarray
{
    int val;
    int array[1]; /* Flexible array member; must be last element of struct */
    int a;
};

string str;

class ta
{
private:
    string str;
    void say1()
    {
        printf("ta private say\n");
    }

protected:
    int pa;

public:
    string Pstr;
    ta()
    {
        // Pstr = str = "init aaaaaaaa";
        // printf("%s\n", Pstr.c_str());
    }
    ~ta()
    {
        // printf("~~~~a\n");
    }

    virtual void say() = 0;
    // {
    //     printf("ta say------aaaa\n");
    // }
};

class tb : private ta
{
private:
    string strb;

protected:
    int ib;

public:
    tb()
    {
        strb = "init strb";
        pa = 1;
        printf("%s, \n", strb.c_str());
    }
    ~tb()
    {
        printf("~~~~b\n");
    }
    void say()
    {
        printf("tb say------bbbb\n");
    }
};

void swapBytes(uint16_t* num)
{
    // 使用异或操作交换字节
    *num = (*num >> 8) | (*num << 8);
}

int main()
{
    int a = 5;
    a &= ~(1 | 4);
    printf("xxx=%x\n", a);
    readfile();
    return 0;
    {
        tb t;
        // t.say();
        ta* pp = (ta*)(&t);
        t.~tb();
        // printf("tp=%p\n", &t);
        pp->say();
    }
    char recv[] = { 0x02, 0xfc, 0x01 };
    uint16_t strLen = uint8_t(recv[1]) | uint8_t(recv[2]) << 8;
    printf("strLen = %d\n", strLen);
}
