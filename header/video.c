#include "video.h"
#include "font.h"
#include "mouse.h"
#include "asm.h"

void InitPalette()
{
    static uint8 palette[16 * 3] = {
        0x00, 0x00, 0x00,
        0xff, 0x00, 0x00,
        0x00, 0xff, 0x00,
        0xff, 0xff, 0x00,
        0x00, 0x00, 0xff,
        0xff, 0x00, 0xff,
        0x00, 0xff, 0xff,
        0xff, 0xff, 0xff,
        0xc6, 0xc6, 0xc6,
        0x84, 0x00, 0x00,
        0x00, 0x84, 0x00,
        0x84, 0x84, 0x00,
        0x00, 0x00, 0x84,
        0x84, 0x00, 0x84,
        0x00, 0x84, 0x84,
        0x84, 0x84, 0x84};
    for (int i = 0; i < 16; i++)
    {
        iowrite8(0x3c8, i);
        iowrite8(0x3c9, palette[i * 3]);
        iowrite8(0x3c9, palette[i * 3 + 1]);
        iowrite8(0x3c9, palette[i * 3 + 2]);
    }

    return;
}

void DrawBlock(uint16 x, uint16 y, uint16 width, uint16 height, uint8 *buffer, uint8 color)
{
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            uint32 mem_pos = SCREEN_WIDTH * i + j;
            if (mem_pos < SCREEN_HEIGHT * SCREEN_WIDTH)
            {
                buffer[mem_pos] = color;
            }
        }
    }
}

void CopyToVBE(uint8 *vram)
{
    for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH / 4; i++)
    {
        ((uint32 *)VBE_VRAM)[i] = ((uint32 *)vram)[i];
    }

    DrawChar('I', MouseX, MouseY, VBE_VRAM, 0x00);
    // int mx = MouseX;
    // int my = MouseY;

    // for (int hbit = 0; hbit < 16; hbit++)
    // {
    //     uint16 line = MouseIcon[hbit];
    //     for (int vbit = 15; vbit >= 0; vbit--)
    //     {
    //         if (line & 1)
    //         {
    //             uint32 mem_pos = SCREEN_WIDTH * (hbit + my) + (vbit + mx);
    //             if (mem_pos < SCREEN_HEIGHT * SCREEN_WIDTH)
    //             {
    //                 VBE_VRAM[mem_pos] = 0x0f;
    //             }
    //         }
    //     }
    //     line >>= 1;
    // }
}

void DrawChar(uint8 c, uint16 x, uint16 y, uint8 *buffer, uint8 color)
{
    uint8 *font = font_chars[c];

    for (int hbit = 0; hbit < FONT_HEIGHT; hbit++)
    {
        if (hbit + y >= SCREEN_HEIGHT)
            continue;
        uint8 line = font[hbit];
        for (int vbit = FONT_WIDTH - 1; vbit >= 0; vbit--, line >>= 1)
        {
            if (vbit + x >= SCREEN_WIDTH)
                continue;
            if (!(line & 1))
                continue;
            uint32 mem_pos = SCREEN_WIDTH * (hbit + y) + (vbit + x);
            if (mem_pos >= SCREEN_HEIGHT * SCREEN_WIDTH)
                continue;
            buffer[mem_pos] = color;
        }
    }
}

void DrawCharMirror(uint8 c, uint16 x, uint16 y, uint8 *buffer, uint8 color)
{
    uint8 *font = font_chars[c];

    for (int hbit = 0; hbit < FONT_HEIGHT; hbit++)
    {
        if (hbit + y >= SCREEN_HEIGHT)
            continue;
        uint8 line = font[hbit];
        for (int vbit = 0; vbit < FONT_WIDTH; vbit++, line >>= 1)
        {
            if (!(line & 1))
                continue;
            uint32 mem_pos = SCREEN_WIDTH * (hbit + y) + (vbit + x);
            if (mem_pos >= SCREEN_HEIGHT * SCREEN_WIDTH)
                continue;
            buffer[mem_pos] = color;
        }
    }
}

void DrawNumber(uint32 num, uint16 x, uint16 y, uint8 *buffer, uint8 color)
{
    uint8 nums[10];
    for (int i = 9; i >= 0; i--)
    {
        nums[i] = num % 10;
        nums[i] += '0';
        num /= 10;
    }

    int s = 0;
    for (; s < 9; s++)
    {
        if (nums[s] != '0')
            break;
    }
    for (; s < 10; s++)
    {
        DrawChar(nums[s], x, y, buffer, color);
        x += FONT_WIDTH;
    }
}

void DrawHex(uint32 num, uint16 x, uint16 y, uint8 *buffer, uint8 color)
{
    uint8 nums[8];
    for (int i = 7; i >= 0; i--)
    {
        nums[i] = num % 16;
        num /= 16;
    }

    int s = 0;
    for (; s < 7; s++)
    {
        if (nums[s] != 0)
            break;
    }
    for (; s < 8; s++)
    {
        if (nums[s] < 10)
            nums[s] += '0';
        else
            nums[s] += 'A' - 10;
        DrawChar(nums[s], x, y, buffer, color);
        x += FONT_WIDTH;
    }
}

void DrawString(uint8 *str, uint16 x, uint16 y, uint8 *buffer, uint8 color)
{
    uint16 tx = x;
    uint16 ty = y;
    for (int i = 0; str[i]; i++)
    {
        if (ty >= SCREEN_HEIGHT)
            break;
        if (str[i] == '\n')
        {
            tx = x;
            ty += FONT_HEIGHT;
            continue;
        }
        DrawChar(str[i], tx, ty, buffer, color);
        tx += FONT_WIDTH;
        if (tx >= SCREEN_WIDTH)
        {
            tx = x;
            ty += FONT_HEIGHT;
        }
    }
}