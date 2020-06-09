[bits 32]

global _start
global _InterruptAsmTemplateStart
global _InterruptAsmTemplateEnd
extern _OrangesMain

_start:

push esi
push edi
call _OrangesMain
jmp $


; jmp后的Bochs断点：b 0x11085
; 键盘中断内存地址：1080
; 键盘中断内存结束：1110
_InterruptAsmTemplateStart:
; jmp template_body
; template_body:
; push es
; push ds
pushad
call GetEIP
add eax,_InterruptAsmTemplateEnd-$
mov ebx,eax
mov eax,[ebx+4]
push eax
mov eax,[ebx]
; sub eax,_InterruptAsmTemplateEnd-$
call eax
pop eax
popad
; pop ds
; pop es
iretd
GetEIP:
pop eax
push eax
ret
_InterruptAsmTemplateEnd:
dd 0x00
dd 0x00

