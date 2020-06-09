
ClearSreen:
push bx
push ds

mov bx,0xb800
mov ds,bx

mov bx,0

loop_clearScreen:
mov word [bx],0x0020
add bx,2
cmp bx,4000
jb loop_clearScreen

pop ds
pop bx

ret

; es:si 文本地址
PrintScreen:
push bx
push ds
push si
push ax

mov bx,0xb800
mov ds,bx

mov bx,0

loop_PrintScreen:
mov al,[es:si]
cmp al,0
jz end_PrintScreen
mov byte [ds:bx],al
inc bx
mov byte [ds:bx],0x07
inc bx
inc si
jmp loop_PrintScreen

end_PrintScreen:

pop ax
pop si
pop ds
pop bx

ret

EnterVGA:
; mov al,0x13
; mov ah,0x00
mov bx,0x4105
mov ax,0x4f02
int 0x10
ret