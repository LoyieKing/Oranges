#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "type.h"

typedef void (*InterruptHandler)(uint32 vector);

void InitInterruptTable();
void SetInterruptHandler(uint16 vector,InterruptHandler handler);

typedef struct
{
    //Lower part of the interrupt function's offset address (also known as pointer).
    uint16 OffsetLow;

    /*Selector of the interrupt function (to make sense - the kernel's selector). 
    The selector's descriptor's DPL field has to be 0 so the iret instruction won't throw a #GP exeption when executed.*/
    uint16 Selector;

    //Have to be 0.
    uint8 Unused;

    //See below
    uint8 Type;

    //Higher part of the offset.
    uint16 OffsetHigh;
} InterruptTableItem;

/*
Name	    Bit	    Full                        Name	Description
P	        7	    Present                     Set to 0 for unused interrupts.
DPL	        5,6	    DescriptorPrivilegeLevel	Gate call protection. Specifies which privilege Level the calling Descriptor minimum should have. So hardware and CPU interrupts can be protected from being called out of userspace.
S	        4	    Storage Segment             Set to 0 for interrupt and trap gates (see below).
Type	    0...3	Gate Type	                Possible IDT gate types :
                                                0b0101  0x5	5	80386 32 bit task gate
                                                0b0110  0x6	6	80286 16-bit interrupt gate
                                                0b0111  0x7	7	80286 16-bit trap gate
                                                0b1110  0xE	14	80386 32-bit interrupt gate
                                                0b1111  0xF	15	80386 32-bit trap gate
*/
#endif