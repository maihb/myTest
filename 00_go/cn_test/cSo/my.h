#ifndef __MYFUNC_H
#define __MYFUNC_H

#include <stdio.h>
#include <stdlib.h>

void say_hello();
int cal_sum(int x, int y);

int setInfo(const char *key, const char *value);
const char *getInfo(const char *key);

#endif