#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "queue.h"
#include "type.h"

typedef struct __key_code
{
    uint8 *ScanCode;
    uint8 *KeyName;
    bool Pressed;
} KeyCode;

extern KeyCode KeyCodes[249];


//return 0xFF00 if key not complete
//return > 0xFF00 stands for unknown key
//the returned value stands for the index of KeyCodes.
uint16 DealKey();

uint16 WaitForKey();

void InitKeyboard();

#endif