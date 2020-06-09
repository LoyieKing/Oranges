InitBPB:
mov [PhysicalDriveNumber],dx
mov dx,0
mov ax,si
sub ax,0x7c00
mov bx,0x200
div bx
;当前:ax:分区开始的扇区数偏移，当前镜像结构内为1
mov [Reserved],ax
call InitCHS
ret


InitCHS:
pusha
mov ah,8
mov dx,[PhysicalDriveNumber]
int 0x13
inc dh
mov byte [NumberOfHead],dh
and cl,0x3f
mov byte [SectorsPerTrack],cl
popa
ret



; ax :LBA Address
; es:bx :buffer address
; cl :Sector count
LBA_Read:
push bx
push cx
; The Algorithm
; LBA is the input value,

; Temp = LBA / (Sectors per Track)
; Sector = (LBA % (Sectors per Track)) + 1
; Head = Temp % (Number of Heads)
; Cylinder = Temp / (Number of Heads)
mov bx,[SectorsPerTrack]
div bl                              ;al:Temp,ah:Sector-1
inc ah
mov cl,ah
mov ah,0                            ;cl:Sector,ax:Temp
mov bx,[NumberOfHead]
div bl                              ;al:cylinder,ah:Head

; Reading sectors with a CHS address
; Cylinder = 0 to 1023 (maybe 4095), Head = 0 to 15 (maybe 254, maybe 255), Sector = 1 to 63

; Set AH = 2
; AL = total sector count (0 is illegal) -- cannot cross ES page boundary, or a cylinder boundary, and must be < 128
; CH = cylinder & 0xff
; CL = Sector | ((cylinder >> 2) & 0xC0);
; DH = Head -- may include two more cylinder bits
; ES:BX -> buffer
; Set DL = "drive number" -- typically 0x80, for the "C" drive
; Issue an INT 0x13.
; The carry flag will be set if there is any error during the read. AH should be set to 0 on success.

; To write: set AH to 3, instead.
mov ch,al
mov dh,ah
mov dl,[PhysicalDriveNumber]
pop ax
pop bx
mov ah,2
int 0x13
ret

