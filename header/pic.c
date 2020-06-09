#include "pic.h"
#include "ioport.h"
#include "asm.h"

void SetInterruptMask0(uint8 mask)
{
    iowrite8(IOPORT_PIC0_DATA, mask);
}

uint8 GetInterruptMask0()
{
    return ioread8(IOPORT_PIC0_DATA);
}

void SetInterruptMask1(uint8 mask)
{
    iowrite8(IOPORT_PIC1_DATA, mask);
}

uint8 GetInterruptMask1()
{
    return ioread8(IOPORT_PIC1_DATA);
}

void InitPIC0(uint8 int_offset)
{
    iowrite8(IOPORT_PIC0_COMMAND, 0x11);    //进入初始化动作
    iowrite8(IOPORT_PIC0_DATA, int_offset); //中断从int_offset开始
    iowrite8(IOPORT_PIC0_DATA, 1 << 2);     //PIC1从IRQ2连接
    iowrite8(IOPORT_PIC0_DATA, 0x01);       //无缓冲区模式
}

void InitPIC1(uint8 int_offset)
{
    iowrite8(IOPORT_PIC1_COMMAND, 0x11);    //进入初始化动作
    iowrite8(IOPORT_PIC1_DATA, int_offset); //中断从int_offset开始
    iowrite8(IOPORT_PIC1_DATA, 2);          //PIC1从IRQ2连接
    iowrite8(IOPORT_PIC1_DATA, 0x01);       //无缓冲区模式
}

void InitAllPIC()
{
    SetInterruptMask0(0xFF);
    SetInterruptMask1(0xFF);

    InitPIC0(0x20);
    InitPIC1(0x28);

    SetInterruptMask0(0xFB);
    SetInterruptMask1(0xFF);

}

bool IsInterruptEnable(uint8 irq)
{
    uint8 allMask;
    if (irq <= 7)
        allMask = GetInterruptMask0();
    else
    {
        allMask = GetInterruptMask1();
        irq -= 8;
    }
    allMask >>= irq;
    return (allMask & 1) ? false : true;
}

void SetInterruptEnable(uint8 irq, bool enable)
{
    if (irq <= 7) //Master PIC
    {
        uint8 preMask = GetInterruptMask0();
        uint8 myMask = 1;
        myMask <<= irq;
        if (enable)
        {
            myMask = ~myMask;
            preMask &= myMask;
        }
        else
        {
            preMask |= myMask;
        }
        SetInterruptMask0(preMask);
        return;
    }

    //Slave PIC

    irq -= 8;
    uint8 preMask = GetInterruptMask1();
    uint8 myMask = 1;
    myMask <<= irq;
    if (enable)
    {
        myMask = ~myMask;
        preMask &= myMask;
    }
    else
    {
        preMask |= myMask;
    }
    SetInterruptMask1(preMask);
}

void EndOfInterrupt(uint8 irq)
{
    if (irq <= 7)
    {
        iowrite8(IOPORT_PIC0_COMMAND, 0x60 + irq);
    }
    else
    {
        iowrite8(IOPORT_PIC1_COMMAND, 0x60 - 8 + irq);
        EndOfInterrupt(IRQ_CASCADE_SLAVE);
    }
}
void EndOfInterrupt0()
{
    iowrite8(IOPORT_PIC0_COMMAND, 0x20);
}
void EndOfInterrupt1()
{
    iowrite8(IOPORT_PIC1_COMMAND, 0x20);
}