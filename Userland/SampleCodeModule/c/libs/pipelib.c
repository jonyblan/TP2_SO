#include <userlandLib.h>
#include <stdint.h>
#include <pipelib.h>


uint8_t pipe_open(const char* name){  // fds[0] = read end, fds[1] = write end
	return (uint8_t)SYSCALL(20, name,0, 0, 0);
}

uint64_t pipe_write(int fd, const char* buf, uint64_t count){
	return (uint64_t)SYSCALL(21, fd, buf, count, 0);
}

uint64_t pipe_read(int fd, char* buf, uint64_t count){
	return (uint64_t)SYSCALL(22, fd, buf, count, 0);
}

void pipe_close(int fd){
	SYSCALL(23, fd, 0, 0, 0);
}

int changeProcessFd(uint16_t pid, uint8_t fd, uint8_t end){
	return SYSCALL(27, pid, fd, end, 0);
}