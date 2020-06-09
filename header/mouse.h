#ifndef MOUSE_H
#define MOUSE_H

int32 MouseX;
int32 MouseY;
bool MouseLButtonPressed;
bool MouseRButtonPressed;
bool MouseMButtonPressed;

void InitMouse();
void DealMouse();

extern uint16 MouseIcon[16];

#endif