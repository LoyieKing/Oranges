#include "type.h"
#include "mouse.h"
#include "pic.h"
#include "ioport.h"
#include "ps2.h"
#include "queue.h"
#include "video.h"
#include "asm.h"
#include "int.h"

Queue dealingMouseQueue;
uint16 MouseIcon[16] = {
    0x8000,
    0xC000,
    0xA000,
    0x9000,
    0x8800,
    0x8400,
    0x8200,
    0x8100,
    0x8080,
    0x87C0,
    0xA200,
    0xD100,
    0x8880,
    0x0500,
    0x0200,
    0x0000};

/*
+---------------
++--------------
+-+-------------
+--+------------
+---+-----------
+----+----------
+-----+---------
+------+--------
+-------+-------
+----+++++------
+-+---+---------
++-+---+--------
+---+---+-------
-----+-+--------
------+---------
----------------
*/

bool mouseInited;
void DealMouse()
{
    if (QueueIsEmpty(&dealingMouseQueue))
        return;
    uint8 data;
    if (!mouseInited)
    {
        QueuePeek(&dealingMouseQueue, &data);
        if (data == 0xFA)
        {
            mouseInited = true;
            QueueDe(&dealingMouseQueue, &data);
        }
        return;
    }

    if (QueueCount(&dealingMouseQueue) < 3)
    {
        return;
    }
    QueueDe(&dealingMouseQueue, &data); //yo_xo_ys_xs_ao_bm_br_bl
    MouseLButtonPressed = data >> 0 & 1;
    MouseRButtonPressed = data >> 1 & 1;
    MouseMButtonPressed = data >> 2 & 1;

    uint8 loc_byte;
    QueueDe(&dealingMouseQueue, &loc_byte);
    int16 rel_x = loc_byte;
    rel_x -= ((data << 4) & 0x100);
    QueueDe(&dealingMouseQueue, &loc_byte);
    int16 rel_y = loc_byte;
    rel_y -= ((data << 3) & 0x100);
    MouseX += rel_x;
    MouseY -= rel_y;
    if (MouseX < 0)
        MouseX = 0;
    else if (MouseX >= SCREEN_WIDTH)
        MouseX = SCREEN_WIDTH - 1;

    if (MouseY < 0)
        MouseY = 0;
    else if (MouseY >= SCREEN_HEIGHT)
        MouseY = SCREEN_HEIGHT - 1;
}

void MouseInterruptHandler(uint32 vector)
{
    while (!PS2STATE_OUTPUT_FULL(ioread8(IOPORT_PS2CONTROLLER_STATE)))
        ;
    uint8 key = ioread8(IOPORT_PS2CONTROLLER_DATA);
    QueueEn(&dealingMouseQueue, key);
    EndOfInterrupt(IRQ_MOUSE);
}

void InitMouse()
{
    dealingMouseQueue.head = 0;
    dealingMouseQueue.tail = 0;

    MouseX = SCREEN_WIDTH / 2;
    MouseY = SCREEN_HEIGHT / 2;
    MouseLButtonPressed = false;
    MouseRButtonPressed = false;
    MouseRButtonPressed = false;

    mouseInited = false; //set it if got 0xFA from port 0x60

    SetInterruptHandler(0x20 + IRQ_MOUSE, MouseInterruptHandler);
    SetInterruptEnable(IRQ_MOUSE, true);

    PS2CONTROLLER_WAIT_FOR_INPUT();
    iowrite8(IOPORT_PS2CONTROLLER_COMMAND, PS2COMMAND_SENDTO_MOUSE);
    PS2CONTROLLER_WAIT_FOR_INPUT();
    iowrite8(IOPORT_PS2CONTROLLER_DATA, MOUSE_COMMAND_ENABLE_DATA_REPORTING);
}