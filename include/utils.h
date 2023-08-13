#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>

#define UNUSED(x) (void)x

const char* ReadFile(const char* path);

unsigned char IsWS(char ch);
unsigned char IsNum(char ch);
unsigned char IsAlpha(char ch);

#endif // UTILS_H_