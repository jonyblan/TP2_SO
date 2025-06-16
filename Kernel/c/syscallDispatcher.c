#include <syscallDispatcher.h>
#include <naiveConsole.h>
#include <stdarg.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <interrupts.h>
#include <registers.h>
#include <time_and_rtc.h>
#include <soundDriver.h>
#include <lib.h>
#include <memoryManager.h>
#include <processManager.h>

#define REGISTERS 18

/*uint64_t syscallDispatcher(uint64_t id, ...)
{
    va_list args;
    va_start(args, id);
    uint64_t ret;
    switch (id)
    {
    case 3:;
    case 4:;
        FileDescriptor fd = va_arg(args, FileDescriptor);
        char *buffer = va_arg(args, char *);
        uint64_t count = va_arg(args, uint64_t);
        if (id == 3)
            ret = read(fd, buffer, count);
        else
            ret = write(fd, buffer, count);
        break;
    case 5:;
        // sys_draw_rectangle
        Rectangle *r = va_arg(args, Rectangle *);
        drawRectangle(r->color, r->up_l_x, r->up_l_y, r->lo_r_x, r->lo_r_y);
        break;
    case 6:;
        // sys_rtc
        TimeStamp *ts = va_arg(args, TimeStamp *);
        getTime(ts);
        ret = ts;
        break;
    case 7:;
        // sys_sleep
        _sti();
        uint32_t ticks = va_arg(args, uint32_t);
        sleep(ticks);
        _cli();
        break;
    case 8:;
        // sys_clear
        vdClear();
        break;
    case 9:;
        // sys_play_sound
        uint64_t sound_ticks = va_arg(args, uint64_t);
        uint64_t hz = va_arg(args, uint64_t);
        _sti();
        beep(hz, sound_ticks);
        _cli();
        break;
    case 10:;
        // sys_change_font
        vdChangeFontSize();
        break;
    case 11:;
        uint64_t *arr = va_arg(args, uint64_t *);
        ret = getRegBackup(arr);
        break;
    case 14:;
		
		void* entryPoint = (void*)va_arg(args, uint64_t);
		//uint8_t argc = (uint8_t)va_arg(args, uint64_t);
		//uint64_t argv_ptr = va_arg(args, uint64_t);
		//char** argv = (char**)argv_ptr;
		char* argv[] = {"startProcess\0", NULL};
		int argc = 2;
		ret = (uint64_t)createProcess(entryPoint, argc, argv);
		break;
	case 13:;
		uint64_t *del = va_arg(args, uint64_t *);
		free(del);
		ret = 0;
		break;
	case 12:;
		uint64_t size = va_arg(args, uint64_t);
		ret = (uint64_t*)malloc(size);
		break;
    case 33:;
        nanoFace();
        break;
    }
    va_end(args);
    return ret;
}*/

uint64_t syscallDispatcher(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    uint64_t ret;
	pid_t auxPid;
    switch (id)
    {
    case 3:;
    case 4:;
        FileDescriptor fd = (FileDescriptor)arg1;
        char *buffer = (char*)arg2;
        uint64_t count = arg3;
        if (id == 3)
            ret = read(fd, buffer, count);
        else
            ret = write(fd, buffer, count);
        break;
    case 5:;
        // sys_draw_rectangle
        Rectangle *r = (Rectangle *)arg1;
        drawRectangle(r->color, r->up_l_x, r->up_l_y, r->lo_r_x, r->lo_r_y);
        break;
    case 6:;
        // sys_rtc
        TimeStamp *ts = (TimeStamp *)arg1;
        getTime(ts);
        ret = ts;
        break;
    case 7:;
        // sys_sleep
        _sti();
        uint32_t ticks = (uint32_t)arg1;
        sleep(ticks);
        _cli();
        break;
    case 8:;
        // sys_clear
        vdClear();
        break;
    case 9:;
        // sys_play_sound
        uint64_t sound_ticks = arg1;
        uint64_t hz = arg2;
        _sti();
        beep(hz, sound_ticks);
        _cli();
        break;
    case 10:;
        // sys_change_font
        vdChangeFontSize();
        break;
    case 11:;
        uint64_t *arr = (uint64_t *)arg1;
        ret = getRegBackup(arr);
        break;
	case 12:;
		uint64_t size = arg1;
		ret = (uint64_t*)malloc(size);
		break;
	case 13:;
		uint64_t *del = (uint64_t *)arg1;
		free(del);
		ret = 0;
		break;
    case 14:;
		void* entryPoint = (void*)arg1;
		uint64_t argc = (uint64_t)arg3; // por alguna razon argc queda en arg3 y no arg2
		char** argv = {""};//(char**) arg3;
		ret = (uint64_t)createProcess(entryPoint, DEFAULT_PRIORITY, argc, argv); //Cablie los parametros, (TO DO) Ponerlos bien
		break;
	case 15:;
		auxPid = (pid_t)arg1;
		ret = (uint64_t)getPriority(auxPid);	
		break;
	case 16:;
		auxPid = (pid_t)arg1;
		uint8_t newPriority = (uint8_t)arg3; // lo mismo que en el CASE 14
		setPriority(auxPid, newPriority);
		ret = 0;
		break;
	case 17:;
		char* name17 = (char*)arg1;
		uint8_t initialValue = (uint8_t)arg3;
		ret = (uint64_t)sem_open(name17, initialValue);
		break;
	case 18:;
		uint8_t postId = (uint8_t)arg1;
		sem_post(postId);
		ret = 0;
		break;
	case 19:;
		uint8_t waitId = (uint8_t)arg1;
		sem_wait(waitId);
		ret = 0;
		break;
	case 20:;
		char* name20 = (char*)arg1;
		int* fds = (int*)arg2;
		ret = pipe_open(name20, fds);
		break;
	case 21:;
		int fd21 = (int)arg1;
		char* buf21 = (char*)arg2;
		uint64_t count21 = (uint64_t)arg3;
		ret = pipe_write(fd21, buf21, count21);
		break;
	case 22:;
		int fd22 = (int)arg1;
		char* buf22 = (char*)arg2;
		uint64_t count22 = (uint64_t)arg3;
		ret = pipe_read(fd22, buf22, count22);
		break;
	case 23:;
		int fd23 = (int)arg1;
		pipe_close(fd23);
		ret = 0;
		break;
	case 24:;
		pid_t pid = (pid_t) arg1;
		killProcess(pid);
		ret = 0;
		break;
	case 25:;
		pid_t pid25 = (pid_t) arg1;
		ret = 0;
		break;
    case 33:;
        nanoFace();
        break;
    }
    return ret;
}

uint64_t read(FileDescriptor fd, char *buffer, uint64_t count)
{
    unsigned char character;
    uint64_t i = 0;
    while (i < count && (character = nextFromBuffer()) != 0)
{
        buffer[i++] = character;
    }
    return i;
}

uint64_t write(FileDescriptor fd, const char *buffer, uint64_t count)
{
    uint32_t styleByFileDescriptor[] = {0, 0x00FFFFFF, 0x00FF0000, 0x0000FF00};
    return vdNPrintStyled(buffer, styleByFileDescriptor[fd], 0, count);
}

uint64_t getRegBackup(uint64_t *arr)
{
    uint64_t *regs = getRegs();
    for (int i = 0; i < REGISTERS; i++)
        arr[i] = regs[i];
    return isBackupDone();
}