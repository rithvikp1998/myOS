#ifndef MYLIBC_H
#define MYLIBC_H

#include <stddef.h>

size_t strlen(const char* str);

void printx(uint64_t data);
void printd(uint64_t data);
void printb(uint64_t data);

#endif