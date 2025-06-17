#ifndef VIDEO_LIB_H
#define VIDEO_LIB_H

#include <stdint.h>
#include <memStatus.h>
int memStatus(MemStatus *ms);

typedef struct{
    uint16_t up_l_x;
    uint16_t up_l_y;
    uint16_t lo_r_y;
    uint16_t lo_r_x;
	uint32_t color;
} Rectangle;
/* 
void drawRectangle(uint32_t color, uint16_t up_l_x, uint16_t up_l_y, uint16_t lo_r_y, uint16_t lo_r_x); */

void clearScreen();

#endif