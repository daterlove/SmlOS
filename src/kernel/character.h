#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "common.h"

void character_print_color(
    uint32* vram,
    uint32 screen_x,
    uint32 pos_x,
    uint32 pos_y,
    char value,
    uint32 color
);

#endif