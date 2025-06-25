#ifndef USERLAND_LIB_H
#define USERLAND_LIB_H

#include <stdint.h>

uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx, uint64_t r8);

#define SYSCALL(id, a1, a2, a3, a4) syscall((id), (uint64_t)(a1), (uint64_t)(a2), (uint64_t)(a3), (uint64_t)(a4))

#endif