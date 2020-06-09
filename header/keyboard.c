#include "keyboard.h"
#include "type.h"
#include "pic.h"
#include "int.h"
#include "queue.h"
#include "asm.h"
#include "ioport.h"
#include "ps2.h"

Queue dealingKeyQueue;
buffer_size buffer[6];

uint16 WaitForKey()
{
    cli();
    while (QueueIsEmpty(&dealingKeyQueue))
    {
        sti();
        hlt();
        cli();
    }
    uint16 key = DealKey();
    sti();
    if (key == 0xFF00)
        return WaitForKey();
    return key;
}

//return 0xFF00 if key not complete
//return > 0xFF00 stands for unknown key
//the returned value stands for the index of KeyCodes.
uint16 DealKey()
{
    QueuePeek(&dealingKeyQueue, buffer);
    //QueueEn(&dealingKeyQueue, key);
    if (buffer[0] != 0xE0 && buffer[0] != 0xE1)
    {
        QueueDeSome(&dealingKeyQueue, 1);
        if (buffer[0] == 0)
            return 0xFF00;
        if (buffer[0] <= 0x53)
            return buffer[0] - 1;
        if (buffer[0] == 0x57)
            return 83;
        if (buffer[0] == 0x58)
            return 87;
        if (buffer[0] >= 0x81 && buffer[0] <= 0xD3)
            return buffer[0] - 44;
        if (buffer[0] == 0XD7)
            return 167;
        if (buffer[0] == 0xD8)
            return 168;
        return 0xFF00 | buffer[0];
    }

    // if (!QueuePeekSome(&dealingKeyQueue, 2, buffer))
    // {
    //     return 0xFF00;
    // }
    cli();
    while (!QueuePeekSome(&dealingKeyQueue, 2, buffer))
    {
        sti();
        hlt();
        cli();
    }
    sti();
    if (buffer[0] == 0xE1)
    {
        // if (!QueuePeekSome(&dealingKeyQueue, 3, buffer))
        // {
        //     return 0xFF00;
        // }
        cli();
        while (!QueuePeekSome(&dealingKeyQueue, 3, buffer))
        {
            sti();
            hlt();
            cli();
        }
        sti();

        if (buffer[1] == 0x1D && buffer[2] == 0x45)
        {
            if (QueuePeekSome(&dealingKeyQueue, 6, buffer))
            {
                if (buffer[3] == 0xE1 && buffer[4] == 0x9D && buffer[5] == 0xC5)
                {
                    QueueDeSome(&dealingKeyQueue, 6);
                    return 248; //Pause
                }
            }
        }
        QueueDeSome(&dealingKeyQueue, 3);
        //Unknown 0xE1 0x?? 0x?? KeyScanCode
        return 0xFFFF;
    }
    if (buffer[0] == 0xE0)
    {
        if (buffer[1] == 0x2A) //Print Screen Pressed
        {
            if (QueuePeekSome(&dealingKeyQueue, 4, buffer))
            {
                if (buffer[2] == 0xE0 && buffer[3] == 0x37)
                {
                    QueueDeSome(&dealingKeyQueue, 6);
                    return 246;
                }
                QueueDeSome(&dealingKeyQueue, 2);
                //unknown key
                return 0xFFFF;
            }
            //Print Screen Pressed not complete yet.
            return 0xFF00;
        }
        else if (buffer[1] == 0xB7) //Print Screen Released
        {
            if (QueuePeekSome(&dealingKeyQueue, 4, buffer))
            {
                if (buffer[2] == 0xE0 && buffer[3] == 0xAA)
                {
                    QueueDeSome(&dealingKeyQueue, 4);
                    return 247;
                }
                QueueDeSome(&dealingKeyQueue, 2);
                //unknown key
                return 0xFFFF;
            }
            //Print Screen Released not complete yet.
            return 0xFF00;
        }
        QueueDeSome(&dealingKeyQueue, 2);
        for (int i = 170; i <= 245; i++)
        {
            if (buffer[1] == KeyCodes[i].ScanCode[1])
                return i;
        }
        return 0xFFFF;
    }
}

void KeyboardInterruptHandler(uint32 vector)
{
    //从键盘控制器读取按键。如果按键不读取，键盘将不会产生下一次中断
    while (!PS2STATE_OUTPUT_FULL(ioread8(IOPORT_PS2CONTROLLER_STATE)))
        ;
    uint8 key = ioread8(IOPORT_PS2CONTROLLER_DATA);
    QueueEn(&dealingKeyQueue, key);
    //向8259发送中断结束命令。如果不发送，8259将不会产生下一次中断。
    //iowrite8(0x20, 0x61);
    EndOfInterrupt(IRQ_KEYBOARD);
    //EndOfInterrupt0();
}

void InitKeyboard()
{
    // KeyboardQueue.head = 0;
    // KeyboardQueue.tail = 0;
    dealingKeyQueue.head = 0;
    dealingKeyQueue.tail = 0;

    SetInterruptHandler(0x20 + IRQ_KEYBOARD, KeyboardInterruptHandler);
    SetInterruptEnable(IRQ_KEYBOARD, true);

    PS2CONTROLLER_WAIT_FOR_INPUT();
    iowrite8(IOPORT_PS2CONTROLLER_COMMAND, PS2COMMAND_WRITE_CONFIG_BYTE);
    PS2CONTROLLER_WAIT_FOR_INPUT();
    iowrite8(IOPORT_PS2CONTROLLER_DATA, 0x47);
}