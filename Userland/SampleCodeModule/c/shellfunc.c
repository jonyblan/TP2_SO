#include <nanoshell.h>
#include <standardlib.h>

#define REGISTERS 18
#define NULL_PARAM 0

uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

void getRegisters()
{
    char *regName[] = {
        "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ",
        "RBP: ", "R8: ", "R9: ", "R10: ", "R11: ", "R12: ",
        "R13: ", "R14: ", "R15: ", "RSP: ", "RIP: ", "RFLAGS: "};
    printf("Register Status: ");
    uint64_t regsStatus[REGISTERS];
    uint64_t call = syscall(11, regsStatus, NULL_PARAM, NULL_PARAM);
    if (call == 0)
        printf("Register backup not done. Press ESC to save register status.");
    else
    {
        uint64_t reg;
        for (int i = 0; i < REGISTERS; i++)
        {
            reg = regsStatus[i];
            printf("\n%s %x", regName[i], reg);
        }
    }
}

void changeFontSize()
{
    syscall(8, NULL_PARAM, NULL_PARAM, NULL_PARAM);  // Syscall clear
    syscall(10, NULL_PARAM, NULL_PARAM, NULL_PARAM); // Syscall change_font_size
}

void printCurrentTime()
{
    TimeStamp ts = {0};
    syscall(6, &ts, NULL_PARAM, NULL_PARAM);
    printf("Current date: %d-%d-%d\nCurrent time: %d:%d:%d hs", ts.day, ts.month, ts.year, ts.hours, ts.minutes, ts.seconds);
}