#ifndef STANDARD_LIB_H
#define STANDARD_LIB_H

#include <stdint.h>

typedef uint64_t pid_t;

typedef enum
{
    STDOUT = 1,
    STDERR,
    STDMARK,
} FileDescriptor;

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint16_t year;
    uint8_t month;
    uint8_t day;
} TimeStamp;

TimeStamp* getTime();
uint64_t fdprintf(FileDescriptor fd, const char *fmt, ...);
uint64_t printf(const char *fmt, ...);
uint64_t scanf(const char *fmt, ...);
uint64_t readLine(char buff[], uint64_t count);
uint64_t putChar(uint64_t character);
uint8_t getChar();
uint8_t getc();
int strcmp(char *s1, char *s2);
void toMinus(char *str);
uint64_t getNextToRead(char *c);
void beep(uint32_t hz, uint32_t ticks);
void* malloc(uint64_t size);
void free(uint64_t* ptr);
int testMalloc();
pid_t createProcess(void* entryPoint, uint64_t argc, char *argv[]);
int getPriority(pid_t pid);
void setPriority(pid_t pid, int newPriority);
#endif