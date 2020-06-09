#include "int.h"
#include "type.h"
#include "asm.h"
#include "video.h"

extern uint8 InterruptAsmTemplateStart;
extern uint8 InterruptAsmTemplateEnd;

uint8 *asmStart;
uint32 asmLength;

#define interruptAsmTable ((uint8 *)0x10000)
#define interruptTable ((InterruptTableItem *)0xF800)

void SetInterruptTableItem(InterruptTableItem *dest, uint16 selector, uint32 offset, uint8 type)
{
    dest->OffsetLow = offset & 0xFFFF;
    dest->OffsetHigh = (offset >> 16) & 0xFFFF;
    dest->Selector = selector;
    dest->Type = type;
    dest->Unused = 0;
}

void EmptyHandler(uint32 vector)
{
    DrawBlock(0, SCREEN_HEIGHT - 16, 8 * 3, 16, videoBuffer, 0xff);
    DrawNumber(vector, 0, 16, videoBuffer, 0x0f);
    CopyToVBE(videoBuffer);
}

void InitInterruptTable()
{
    asmLength = (&InterruptAsmTemplateEnd) - (&InterruptAsmTemplateStart);
    asmStart = &InterruptAsmTemplateStart;
    for (int i = 0; i < 256; i++)
    {
        uint8 *asmAddress = interruptAsmTable + i * 128;
        for (int i = 0; i < asmLength; i++)
        {
            asmAddress[i] = asmStart[i];
        }
        uint32 *dataAddress = (uint32 *)(asmAddress + asmLength);
        dataAddress[0] = (uint32)EmptyHandler;
        dataAddress[1] = i;
    }
    for (int i = 0; i < 256; i++)
    {
        SetInterruptTableItem(interruptTable + i, 8 << 3, interruptAsmTable + i * 128, 0x8E);
    }
}

void SetInterruptHandler(uint16 vector,InterruptHandler handler)
{
    InterruptHandler* address = (InterruptHandler*)(interruptAsmTable+128*vector+asmLength);
    *address = handler;
}
