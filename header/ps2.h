#ifndef PS2_H
#define PS2_H

#define PS2CONTROLLER_WAIT_FOR_INPUT() while (PS2STATE_INPUT_FULL(ioread8(IOPORT_PS2CONTROLLER_STATE)))

/*
uint8 from 0x64

bit Meaning
0	Output buffer status (0 = empty, 1 = full)
(must be set before attempting to read data from IO port 0x60)

1	Input buffer status (0 = empty, 1 = full)
(must be clear before attempting to write data to IO port 0x60 or IO port 0x64)

2	System Flag
Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)

3	Command/data (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
4	Unknown (chipset specific)
May be "keyboard lock" (more likely unused on modern systems)

5	Unknown (chipset specific)
May be "receive time-out" or "second PS/2 port output buffer full"

6	Time-out error (0 = no error, 1 = time-out error)
7	Parity error (0 = no error, 1 = parity error)
*/

/*  must be set before attempting to read data from IO port 0x60    */
#define PS2STATE_OUTPUT_FULL(state_pack) ((state_pack >> 0) & 1)
/*  must be clear before attempting to write data to IO port 0x60 or IO port 0x64   */
#define PS2STATE_INPUT_FULL(state_pack) ((state_pack >> 1) & 1)
/* Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)    */
#define PS2STATE_SYSTEM_FLAG(state_pack) ((state_pack >> 2) & 1)
/*  0 = data written to input buffer is data for PS/2 device, 
    1 = data written to input buffer is data for PS/2 controller command   */
#define PS2STATE_COMMAND_DATA(state_pack) ((state_pack >> 3) & 1)
/*  chipset specific
    May be "keyboard lock" (more likely unused on modern systems)  */
#define PS2STATE_UNKNOWN4(state_pack) ((state_pack >> 4) & 1)
/*  chipset specific
    May be "receive time-out" or "second PS/2 port output buffer full"  */
#define PS2STATE_UNKNOWN5(state_pack) ((state_pack >> 5) & 1)
/*  Time-out error 
    0 = no error, 
    1 = time-out error   */
#define PS2STATE_TIMEOUT_ERROR(state_pack) ((state_pack >> 6) & 1)
/*  Parity error 
    0 = no error, 
    1 = parity error   */
#define PS2STATE_PARITY_ERROR(state_pack) ((state_pack >> 7) & 1)

/*  Bit	Meaning
    0	First PS/2 port interrupt (1 = enabled, 0 = disabled)
    1	Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported)
    2	System Flag (1 = system passed POST, 0 = your OS shouldn't be running)
    3	Should be zero
    4	First PS/2 port clock (1 = disabled, 0 = enabled)
    5	Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2 PS/2 ports supported)
    6	First PS/2 port translation (1 = enabled, 0 = disabled)
    7	Must be zero*/
#define PS2COMMAND_READ_CONFIG_BYTE 0x20
/*  Bit	Meaning
    0	First PS/2 port interrupt (1 = enabled, 0 = disabled)
    1	Second PS/2 port interrupt (1 = enabled, 0 = disabled, only if 2 PS/2 ports supported)
    2	System Flag (1 = system passed POST, 0 = your OS shouldn't be running)
    3	Should be zero
    4	First PS/2 port clock (1 = disabled, 0 = enabled)
    5	Second PS/2 port clock (1 = disabled, 0 = enabled, only if 2 PS/2 ports supported)
    6	First PS/2 port translation (1 = enabled, 0 = disabled)
    7	Must be zero*/
#define PS2COMMAND_WRITE_CONFIG_BYTE 0x60
#define PS2COMMAND_SENDTO_MOUSE 0xD4

#define MOUSE_COMMAND_RESET 0xFF
#define MOUSE_COMMNAD_RESEND 0xFE
#define MOUSE_COMMAND_SET_DEFAULT 0xF6
#define MOUSE_COMMAND_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_COMMAND_ENABLE_DATA_REPORTING 0xF4
/*valid values are 10, 20, 40, 60, 80, 100, and 200.*/
#define MOUSE_COMMAND_SAMPLE_RATE 0xF3
/*see https://wiki.osdev.org/%228042%22_PS/2_Controller#Detecting_PS.2F2_Device_Types */
#define MOUSE_COMMAND_GET_DEVICE_ID 0xF2
#define MOUSE_COMMAND_SET_REMOTE_MODE 0xF0
#define MOUSE_COMMAND_SET_WRAP_MODE 0xEE
#define MOUSE_COMMAND_RESET_WRAP_MODE 0xEC
#define MOUSE_COMMAND_READ_DATA 0xEB
#define MOUSE_COMMAND_SET_STREAM MODE 0xEA
#define MOUSE_COMMAND_STATUS_REQUEST 0xE9
/*00:1count/mm;01:2cout/mm;02:4count/mm;03:8count/mm*/
#define MOUSE_COMMAND_SET_RESOLUTION 0xE8
/*1 or 2*/
#define MOUSE_COMMAND_SET_SCALING 0xE6

#endif