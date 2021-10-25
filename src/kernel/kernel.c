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

    character_print_color(addr, 1440, 100, 200, 'a', 0x00000000);
    character_print_color(addr, 1440, 101, 200, 'a', 0x00000000);
    while (1)
    {

    }
}
