#include "my.h"

// extern int gNum = 0;
int gNum = 0;
char *value = "c return str";
void say_hello()
{
    printf("hello world\n");
}

int cal_sum(int x, int y)
{
    return x + y;
}
int setInfo(const char *key, const char *value)
{
    gNum++;
    printf("C---- key[%s], value[%s] gNum[%d] \n", key, value, gNum);
    return 1;
}
const char *getInfo(const char *key)
{
    return value;
}