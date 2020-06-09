#ifndef PIC_H
#define PIC_H

#include "type.h"

#define IRQ_SYSTEM_TIMER    0
#define IRQ_KEYBOARD        1
#define IRQ_CASCADE_SLAVE   2
#define IRQ_FLOPPY_DISK     6
#define IRQ_RTC             8
#define IRQ_EVGA            9
#define IRQ_MOUSE           12
#define IRQ_DMA             13
#define IRQ_HD_CTRL_MASTER  14
#define IRQ_HD_CTRL_SLAV    15


//Set PIC0(Master PIC)'s interrupt mask
//1-disable,0-enable
//i.e. 0xfe:disable all but pic1
void SetInterruptMask0(uint8 mask);
//see SetInterruptMask0
uint8 GetInterruptMask0();
void SetInterruptMask1(uint8 mask);
uint8 GetInterruptMask1();

//Init PIC0(Master PIC),please call SetInterruptMask0 to disable all int before call init.
//int_offset:i.e. if int_offset is 0x20, IRQ3 where raise interrupt 0x23(0x20+3)
void InitPIC0(uint8 int_offset);
//see InitPic0
void InitPIC1(uint8 int_offset);

void InitAllPIC();

bool IsInterruptEnable(uint8 irq);
void SetInterruptEnable(uint8 irq, bool enable);

void EndOfInterrupt(uint8 irq);
void EndOfInterrupt0();
void EndOfInterrupt1();


#endif