#include "common.h"
#include "character.h"
#include "interrupt.h"

struct memory_info_t
{
    uint32 address1;
    uint32 address2;
    uint32 length1;
    uint32 length2;
    uint32 type;
};

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

    char buf[90] = {0};
    snprintf(buf, sizeof(buf), "info: mode:%d, screen_x:%d, screen_y:%d, vram:%p end",
        *mode, *screen_x, *screen_y, *vram);

    string_print_color(addr, 1440, 0, 0, buf, 0x00000000);
    idt_init();

    string_print_color(addr, 1440, 101, 244, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0x00000000);

    struct memory_info_t* memory_info = (struct memory_info_t*)0xffff800000007e00;

    int y = 264;
    for (i = 0; i < 8; i++)
    {
        snprintf(buf, sizeof(buf), "info: addr1:%x, addr2:%d, len1:%d, len2:%d, type:%d",
            memory_info->address1, memory_info->address2,
            memory_info->length1, memory_info->length2, memory_info->type);
        string_print_color(addr, 1440, 101, y, buf, 0x00000000);
        y += 16;
        memory_info++;
    }


    

    


    i = 0;
    //int j = 10 / i;

    while (1)
    {

    }
}
