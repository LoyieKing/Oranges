; ax :FAT扇区偏移
GetFatAddress:
mov ax,[Reserved]
mov bx,[ReserverdSector]
add ax,bx
ret

;ax:Root Entries
GetRootEntries:
;Root Entries:ReserverdSector+SectorsPerFAT*NumberofFAT+offset
mov dx,0
mov ax,[SectorsPerFAT]
mov bl,[NumberofFAT]
mul bl
mov bx,[ReserverdSector]
add ax,bx
mov bx,[Reserved]
add ax,bx
;inc ax

ret

;input:
; ds:di :寻找的文件名，长度11（即包括扩展名）
; es:si :目录表项FDT所在的位置
; ax : 最大地址
;output:
; es:si :表项地址（如果存在）
; al : 1表示成功，0表示失败
FindFile:
mov bx,0
loop_FindFile:
mov cl,[ds:bx+di]
mov ch,[es:bx+si]
cmp cl,ch
jne next_FindFile
inc bx
cmp bx,11
jae success_FindFile
jmp loop_FindFile
next_FindFile:
add si,0x20
mov bx,0
cmp ax,si
jb loop_FindFile
;jmp fail_FindFile
fail_FindFile:
mov al,0
ret

success_FindFile:
mov al,1
ret

;input:
; ax : 簇号,必须>=2
; es:bx :buffer address
ReadClusterData:
push ds
push bx
mov bx,0
mov ds,bx
sub ax,2
mov bx,0
mov bl,[SectorsPerCluster]
push bx
mul bx
mov cx,ax
; cx : 簇号->扇区偏移

call GetRootEntries
add cx,ax
mov ax,[RootEntries]
mov bx,32
mul bx
;mov dx,0
mov bx,512
div bx
add ax,cx
; ax : 簇号->磁盘扇区偏移

pop cx                          ;SectorsPerCluster
pop bx                          ;buffer address
call LBA_Read
pop ds
ret

; ds:0x00 : FAT位置
; si : 簇号
ReadClusterNext:
push ax
push bx
push dx

mov ax,si
mov bx,2
mul bx
mov bx,ax
mov si,[ds:bx]

pop dx
pop bx
pop ax

ret

