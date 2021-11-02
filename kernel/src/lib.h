#ifndef _LIB_H_
#define _LIB_H_
#include "types.h"
#include <stdarg.h>

void itoa(unsigned int n, char* buf);
int atoi(char* pstr);
void xtoa(unsigned int n, char * buf);
int isdigit(unsigned char c);
int isletter(unsigned char c);
int toupper(int c);
void* memset(void* s, int c, unsigned int count);
char* strcpy(char* dest, const char* src);
unsigned int strlen(const char* s);
char* strcat(char* dest, const char* src);
int snprintf(char *string, size_t length, const char *format, ...);
void *memcpy(void *dest, const void *src, size_t n);

#endif