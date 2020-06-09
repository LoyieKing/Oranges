#ifndef VIDEO_H
#define VIDEO_H

#include "type.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

uint8 videoBuffer[SCREEN_HEIGHT * SCREEN_WIDTH];

// #define ScreenWidth 320
// #define ScreenHeight 200
//Copy vram buffer to VRAM(320*200 , 62KB , 0xfa00)
//extern void CopyToVRAM(uint8 *vram);

#define VBE_VRAM ((uint8 *)0xe0000000)
void CopyToVBE(uint8 *vram);

void InitPalette();

void DrawBlock(uint16 x, uint16 y, uint16 width, uint16 height, uint8 *buffer, uint8 color);
void DrawChar(uint8 c, uint16 x, uint16 y, uint8 *buffer, uint8 color);
void DrawCharMirror(uint8 c, uint16 x, uint16 y, uint8 *buffer, uint8 color);
void DrawNumber(uint32 num, uint16 x, uint16 y, uint8 *buffer, uint8 color);
void DrawHex(uint32 num, uint16 x, uint16 y, uint8 *buffer, uint8 color);
void DrawString(uint8 *str, uint16 x, uint16 y, uint8 *buffer, uint8 color);

#define COLOR_BLACK         0x0
#define COLOR_REG           0x1
#define COLOR_GREEN         0x2
#define COLOR_YELLOW        0x3
#define COLOR_BLUE          0x4
#define COLOR_PURPLE        0x5
#define COLOR_CYAN          0x6
#define COLOR_WHITE         0x7
#define COLOR_LIGHT_GRAY    0x8
#define COLOR_DARK_RED      0x9
#define COLOR_DARK_GREEN    0xa
#define COLOR_DARK_YELLOW   0xb
#define COLOR_DARK_BLUE     0xc
#define COLOR_DARK_PURPLE   0xd
#define COLOR_DARK_CYAN     0xe
#define COLOR_GRAY          0xf

#endif