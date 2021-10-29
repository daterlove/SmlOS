#include "common.h"
#include "ascii_font.h"

bool character_print_color(
    uint32* vram,
    uint32 screen_x,
    uint32 pos_x,
    uint32 pos_y,
    char value,
    uint32 color
)
{
    if (pos_x + 9 > screen_x)
    {
        return false;
    }

    uint32 i;
    uint32* ptr = vram + (pos_y * screen_x) + pos_x;
    uint16* line = (uint16*)(g_console_font_9x16 + (value * 2 * 16));
    for (i = 0; i < 16; i++)
    {
        register uint16 data = *line;
        if ((data & 0x80) != 0)
        {
            ptr[0] = color;
        }
        if ((data & 0x40) != 0)
        {
            ptr[1] = color;
        }
        if ((data & 0x20) != 0)
        {
            ptr[2] = color;
        }
        if ((data & 0x10) != 0)
        {
            ptr[3] = color;
        }
        if ((data & 0x08) != 0)
        {
            ptr[4] = color;
        }
        if ((data & 0x04) != 0)
        {
            ptr[5] = color;
        }
        if ((data & 0x02) != 0)
        {
            ptr[6] = color;
        }
        if ((data & 0x01) != 0)
        {
            ptr[7] = color;
        }
        if ((data & 0x8000) != 0)
        {
            ptr[8] = color;
        }

        line++;
        ptr += screen_x;
    }
    return true;
}

void string_print_color(
    uint32* vram,
    uint32 screen_x,
    uint32 pos_x,
    uint32 pos_y,
    char* str,
    uint32 color
)
{
    if (str == nullptr)
    {
        return;
    }
    char* ptr = str;
    while (*ptr != '\0')
    {
        character_print_color(
            vram, screen_x, pos_x, pos_y, *ptr, color
        );
        pos_x += 9;
        ptr++;
    }
    return;
}