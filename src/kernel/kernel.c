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

    char buf[50] = {0};
    snprintf(buf, sizeof(buf), "buf:%s, ptr:%p end", "qrstuvwxyz", 0x100);

    string_print_color(addr, 1440, 101, 200, buf, 0x00000000);
    //string_print_color(addr, 1440, 101, 224, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0x00000000);

    while (1)
    {

    }
}
