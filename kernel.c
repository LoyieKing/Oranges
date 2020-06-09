#include "header\type.h"
#include "header\video.h"
#include "header\asm.h"
#include "header\font.h"
#include "header\pic.h"
#include "header\int.h"
#include "header\keyboard.h"
#include "header\queue.h"
#include "header\mouse.h"

void InitAll()
{
    InitPalette();

    InitAllPIC(); //Unknown bug in mingw-gcc -O3
    InitInterruptTable();
    InitKeyboard();
    //InitMouse();

    DrawBlock(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, videoBuffer, 0xff);
    CopyToVBE(videoBuffer);

    sti();
}

void RunMain();

void OrangesMain(uint32 PartitionOffset, uint32 VolumeSerialNumber)
{

    InitAll();

    RunMain();

    DrawBlock(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, videoBuffer, 0xff);
    DrawString("Program finished.", 0, 0, videoBuffer, 0x1);
    CopyToVBE(videoBuffer);
    while (true)
    {
        hlt();
    }
}