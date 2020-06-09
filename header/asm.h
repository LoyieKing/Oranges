#ifndef ASM_H
#define ASM_H

#include "type.h"

#define hlt() __asm__("hlt")
#define cli() __asm__("cli")
#define sti() __asm__("sti")
#define stihlt() __asm__("sti;hlt;")
#define int3() __asm__("int3")

#define ioread8(port)                 \
    ({                                \
        uint16 __ioread8port = port;  \
        uint8 __ioread8result;        \
        __asm__(                      \
            "mov %1,%%dx        \n\t" \
            "in %%dx,%%al       \n\t" \
            "mov %%al,%0        \n\t" \
            : "=r"(__ioread8result)   \
            : "r"(__ioread8port)      \
            : "ax", "dx");            \
        __ioread8result;              \
    })

#define iowrite8(port, val)                             \
    ({                                                  \
        uint16 __iowrite8port = port;                   \
        uint8 __iowrite8val = val;                      \
        __asm__(                                        \
            "mov %0,%%al    \n\t"                       \
            "mov %1,%%dx    \n\t"                       \
            "out %%al,%%dx  \n\t" ::"r"(__iowrite8val), \
            "r"(__iowrite8port)                         \
            : "ax", "dx");                              \
    })

//WARNING! : mov to not-sreg may cause unknown bugs,becasue C-compliler DO NOT know the reg has been changed
#define asm_mov16(sreg, val)                        \
    {                                               \
        uint16 __asm_mov16val = val;                \
        __asm__(                                    \
            "mov %0,%%sreg" ::"r"(__asm_mov16val)); \
    }



#endif