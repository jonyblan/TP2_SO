#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>

uint8_t pipe_open(const char* name);

uint64_t pipe_write(int fd, const char* buf, uint64_t count);

uint64_t pipe_read(int fd, char* buf, uint64_t count);

void pipe_close(int fd);

int changeProcessFd(uint16_t pid, uint8_t fd, uint8_t end);

#endif