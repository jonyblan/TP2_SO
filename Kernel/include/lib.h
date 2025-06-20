#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>          /* define size_t */

#ifdef memset
#  undef memset
#endif
#ifdef memcpy
#  undef memcpy
#endif

void *memset(void *destination, int32_t c, size_t len);
void *memcpy(void *destination, const void *source, size_t len);

size_t  strlen(const char *string);
char   *cpuVendor(char *result);
uint8_t inb (uint16_t port);
void    outb(uint16_t port, uint8_t value);
int strcmp(const char *s1, const char *s2);

#endif /* LIB_H */