#include "common.h"

#include "character.h"

void start_kernel(void)
{
    int32* addr = (int *)0xffff800000a00000;
    int i;
    for (i = 0; i < 1440 * 450; i++)
    {
        addr[i] = 0x0099ffcc;
    }

    uint16* mode =  (uint16*)0xffff800000000ff2;
    uint16* screen_x =  (uint16*)0xffff800000000ff4;
    uint16* screen_y =  (uint16*)0xffff800000000ff6;
    uint32* vram =  (uint32*)0xffff800000000ff8;

    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "mode:%d, screen_x:%d, screen_y:%d, vram:%p end",
        *mode, *screen_x, *screen_y, *vram);

    string_print_color(addr, 1440, 0, 0, buf, 0x00000000);
    //string_print_color(addr, 1440, 101, 224, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0x00000000);

    while (1)
    {

    }
}
