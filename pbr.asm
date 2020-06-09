;#7c00
org 0x7c00

jmp entry

OEM_ID:
db "MSDOS5.0"                       ;0x03
times 8-($-OEM_ID) db 0             ;padding to 8 bytes

BPB:
BytesPerSector:     dw 512          ;0x0B 硬件扇区的大小
SectorsPerCluster:  db 2            ;0x0D 一个簇中的扇区数
ReserverdSector:    dw 8            ;0x0E 第一个FAT开始之前的扇区数，包括引导扇区
NumberofFAT:        db 2            ;0x10 该分区上FAT的副本数。本字段的值一般为2
RootEntries:        dw 512          ;0x11 能够保存在该分区的根目录文件夹中的32个字节长的文件和文件夹名称项的总数
SmallSector:        dw 65535        ;0x13 对大于65536个扇区（32MB）的分区来说，本字段的值为0，而使用大扇区数来取代它
MediaDescriptor:    db 0xF8         ;0x15 值0xF8表示硬盘，0xF0表示高密度的3.5寸软盘
SectorsPerFAT:      dw 0x0080       ;0x16 该分区上每个FAT所占用的扇区数
SectorsPerTrack:    dw 0x003F       ;0x18 每道扇区数
NumberOfHead:       dw 0x0004       ;0x1A 磁头数
HiddenSector:       dd 0x00_00_00_01;0x1C 该分区上引导扇区之前的扇区数。在引导序列计算到根目录和数据区的绝对位移的过程中使用了该值
LargeSector:        dd 0x00_00_00_00;0x20 如果小扇区数字段的值不为0，那么本字段的值为0

ExtBPB:
PhysicalDriveNumber:db 0x80         ;0x24 与BIOS物理驱动器号有关
Reserved:           db 0x00         ;0x25 一般为1，当前PBR此处内存结构用来保存MRR至PBR的扇区偏移
ExtBootSignature:   db 0x29         ;0X26 扩展引导标签(Extended Boot Signature) 本字段必须要有能被Windows 2000所识别的值0x28或0x29
VolumeSerialNumber: dd 0xC133EF9E   ;0x27 在格式化磁盘时所产生的一个随机序号，它有助于区分磁盘
VolumeLabel:
db "NO NAME"                        ;0x2B 本字段只能使用一次，它被用来保存卷标号。现在，卷标被作为一个特殊文件保存在根目录中
times 11-($-VolumeLabel) db 0x20    ;padding to 11 bytes
FileSystemType:
db "FAT16"                          ;0x36 根据该磁盘格式，该字段的值可以为FAT、FAT12或FAT16
times 8-($-FileSystemType) db 0     ;padding to 8 bytes


;程序当前状态：
;从MBR引导至了PBR。dl保存启动的磁盘号，ds:si保存着被引导的扇区（内存）偏移
;当前镜像结构中，被引导分区处于第二扇区，即内存偏移数为0x200
;所以ds:si的值应当为0x7c00+0x200=0x7e00
entry:


call ClearSreen

; 初始化,读取FAT根目录
call InitBPB

mov ax,[Reserved]                       ;分区开始的偏移 
inc ax
mov bx,0x7e00
mov cl,1
call LBA_Read

jmp entry_SecondSector


%include "lib/charmode.asm"
%include "lib/disk.asm"
%include "lib/filesystem.asm"

Filename_System:        db "SYSTEM     "
Filename_Kernel:        db "KERNEL  SYS"
Chars_NoSystemFolder:   db "Can't find SYSTEM!",0x00
Chars_NotSystemFolder:  db "SYSTEM type wrong!",0x00
Chars_NoKernelFile:     db "Can't find KERNEL.SYS!",0x00
Chars_NotKernelFile:    db "KERNEL.SYS type wrong!",0x00
Chars_MuchKernelFile:   db "KERNEL.SYS too big!",0x00
times 510-($-$$) db 0               ;填充510字节的0，$$表示编译后的起始地址
dw 0xaa55                           ;表示结束，加上前面的510字节正好512字节满足条件

entry_SecondSector:

call GetRootEntries

mov bx,0x500
mov cl,1
call LBA_Read

; 寻找SYSTEM文件夹
mov di,Filename_System
mov si,0x500
mov ax,0x200
call FindFile
cmp al,0
je fail_nosys
; 判断为文件夹
mov byte al,[es:si+0x0B]
cmp al,0x10
jne fail_notsys
; 进入文件夹
mov ax,[es:si+0x1A]
mov bx,0x500
call ReadClusterData

; 寻找Kernel文件
mov di,Filename_Kernel
mov si,0x500
mov ax,0x400
call FindFile
cmp al,0
je fail_nokernel
; 判断为系统文件
mov byte al,[es:si+0x0B]
cmp al,0x04
jne fail_notkernel
; 判断长度,不得长于0x70000,即448KB
mov dword eax,[es:si+0x1c]
cmp eax,0x70000
ja fail_muchkernel

; 读取FAT至0x80_00~0x1_00_00
call GetFatAddress
mov cx,[SectorsPerFAT]
mov bx,0x8000
call LBA_Read

; 读取文件至0x1_00_00~0x8_00_00
mov ax,[BytesPerSector]
mov bx,0
mov bl,[SectorsPerCluster]
mul bx
push ax
mov bp,sp                       ;栈顶为内存每次加的值（每个簇的byte数）

mov si,[es:si+0x1a]
mov ax,0x800
mov ds,ax                       ;ds指向FAT表

mov ax,0x1000
mov es,ax                       ;es指向读出位置



mov ax,si
mov bx,0
loop_read_kernel:
call ReadClusterData
call ReadClusterNext
cmp si,0xffef
ja read_kernel_end
mov ax,[bp]                         ;[bp]:每个簇的大小(byte)
add bx,ax
jnc not_add_es
mov ax,es
add ax,0x1000
mov es,ax
not_add_es:
mov ax,si
jmp loop_read_kernel


fail_nosys:
mov si,Chars_NoSystemFolder
jmp print_and_loop

fail_notsys:
mov si,Chars_NotSystemFolder
jmp print_and_loop


fail_nokernel:
mov si,Chars_NoKernelFile
jmp print_and_loop


fail_notkernel:
mov si,Chars_NotKernelFile
jmp print_and_loop

fail_muchkernel:
mov si,Chars_MuchKernelFile
jmp print_and_loop

print_and_loop:
call PrintScreen
loop_hlt:
hlt
jmp loop_hlt


read_kernel_end:
;文件已经被读取到了0x10000
cli                                     ;禁用中断

call EnterVGA

;建立GDT


;31-------------------16 15------------------0
; |      Base 0:15      |     Limit 0:15     |
;63-------56 55-52 51-48 47------40 39------32
; |  Base   |Flags|Limit|AccessByte|  Base   |
; |  24:31  |     |16:19|          |  16:23  |
; --------------------------------------------


;AccessByte:
; Pr Privi S Ex DC RW AC
; 7   6 5  4 3  2  1  0
; Pr: Present bit. This must be 1 for all valid selectors.
; Privl: Privilege, 2 bits. Contains the ring level, 0 = highest (kernel), 3 = lowest (user applications).
; S: Descriptor type. This bit should be set for code or data segments and should be cleared for system segments (eg. a Task State Segment)
; Ex: Executable bit. If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
; DC: Direction bit/Conforming bit.
;   Direction bit for data selectors: Tells the direction. 0 the segment grows up. 1 the segment grows down, ie. the offset has to be greater than the limit.
;   Conforming bit for code selectors:
;       If 1 code in this segment can be executed from an equal or lower privilege level. For example, code in ring 3 can far-jump to conforming code in a ring 2 segment. The privl-bits represent the highest privilege level that is allowed to execute the segment. For example, code in ring 0 cannot far-jump to a conforming code segment with privl==0x2, while code in ring 2 and 3 can. Note that the privilege level remains the same, ie. a far-jump form ring 3 to a privl==2-segment remains in ring 3 after the jump.
;       If 0 code in this segment can only be executed from the ring set in privl.
; RW: Readable bit/Writable bit.
;   Readable bit for code selectors: Whether read access for this segment is allowed. Write access is never allowed for code segments.
;   Writable bit for data selectors: Whether write access for this segment is allowed. Read access is always allowed for data segments.
; Ac: Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.

;Flags:
; Gr Sz 0 0
; 3  2  1 0
; Gr: Granularity bit. If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks (page granularity).
; Sz: Size bit. If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode. You can have both 16 bit and 32 bit selectors at once.


;保护模式下的内存布局安排：
;     GDT      IDT           ReadedKernelCode             Stack          VRAM
; |---------|------|--------------------------------|-----------------|--------|
;0x0000 0xF800 0x10000                          0x80000           0xa0000 0Xafa00
;    62KB     2KB                448KB                    128KB         62KB   

mov bx,0
mov ds,bx

;X86要求，第0项为全0
mov dword [ds:bx+0x00],0x00
mov dword [ds:bx+0x04],0x00

;01:GDT的内存位置即0x0000~0xF7FF 
;   Base:0x0000_0000 Limit:0x0_F7FF
mov dword [ds:bx+0x08],0x0000_F7FF
mov dword [ds:bx+0x0c],0x00_4_0_92_00

;02:IDT即0xF800~0xFFFF
;   Base:0x0000_F800 Limit:0x0_07FF
mov dword [ds:bx+0x10],0xF800_07FF
mov dword [ds:bx+0x14],0x00_4_0_92_00

;03:启动的代码段 0x10000~0x7FFFF
;   Base:0x0001_0000 Limit:0x6_FFFF
mov dword [ds:bx+0x18],0x0000_FFFF
mov dword [ds:bx+0x1c],0x00_4_6_92_01

;04:栈段 0x80000~0x9FFFF
;   Base:0x000A_0000 Limit:0x1_FFFF
mov dword [ds:bx+0x20],0x0000_FFFF
mov dword [ds:bx+0x24],0x00_4_1_96_0A

;05:VRAM
;   Base:0x000a_0000 Limit:0x0_FA00
mov dword [ds:bx+0x28],0x0000_FA00
mov dword [ds:bx+0x2c],0x00_4_0_92_0A

;06:Flat Mode
;   Base:0x0000_0000 Limit:0xFFFF_FFFF
mov dword [ds:bx+0x30],0x0000_FFFF
mov dword [ds:bx+0x34],0x00_C_F_92_00

;07:Rest RAM-Data
;   Base:0x0000_0000 Limit:0xFFFF_FFFF
mov dword [ds:bx+0x38],0x0000_FFFF
mov dword [ds:bx+0x3c],0x00_C_F_92_00

;08:Rest RAM-Code
;   Base:0x0000_0000 Limit:0xFFFF_FFFF
mov dword [ds:bx+0x40],0x0000_FFFF
mov dword [ds:bx+0x44],0x00_C_F_9A_00

mov word [GDT_Size],8*9-1
lgdt [GDT_Size]
lidt [IDT_Size]

;保存硬盘序列号、分区偏移
mov esi,[VolumeSerialNumber]
mov edi,0
mov di,[Reserved]


in al,0x92
or al,0000_0010B
out 0x92,al                             ;打开A20

mov eax,cr0
or eax,1
mov cr0,eax                             ;设置PE位


;设置栈段选择子：
mov ax,0b111_000
mov ss,ax

mov esp,0xa0000
mov ebp,0xa0000


mov ax,0b111_000
mov ds,ax
mov ax,0b011_000
mov es,ax

mov ebx,0x00

copy_kernel:
mov eax,[es:ebx]
mov [ds:ebx+0x100000],eax
add ebx,4
cmp ebx,0x70000
jb copy_kernel

mov ax,0b111_000
mov es,ax


;进入载入的程序
jmp 0b1000_000:real_jmp

[bits 32]
real_jmp:
jmp 0b1000_000:0x100000

GDT_Size:               dw 0x00
GDT_Offset:             dd 0x00000000
IDT_Size:               dw 256*8-1
IDT_Offset:             dd 0x0000f800
