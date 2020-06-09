#mingw64-make

#系统名
OS_NAME = oranges

#原镜像名
PURE_IMG = .\.prebuilt\pure.img

#输出目录
OUT = .\.out

#编译工具
NASM = D:\Desktop\DOS\nasm2.8\nasm.exe
DD = .\.prebuilt\dd.exe
CC = gcc -c -ffreestanding -m32 -march=i386 -mpreferred-stack-boundary=4
LD = wsl ld -Ttext 0x100000 --oformat binary -m elf_i386
FAT16 = .\.prebuilt\fat16\Fat16.exe

# NASM = nasm
# DD = dd
# CC = gcc
# LD = ld


final_image = $(OUT)\$(OS_NAME).img
pbr_bin = $(OUT)\pbr.bin
kernel_sys = $(OUT)\kernel.sys

$(final_image): $(pbr_bin) $(kernel_sys)
	copy $(PURE_IMG) $(final_image) /Y
	$(DD) if=$(pbr_bin) of=$(final_image) bs=512 count=2 seek=1
	$(FAT16) $(final_image) $(kernel_sys) SYSTEM\KERNEL.SYS
	$(info Image file generate success.)

$(pbr_bin): pbr.asm lib\charmode.asm lib\disk.asm lib\filesystem.asm
	$(NASM) pbr.asm -o $(pbr_bin) -l $(OUT)\pbr.lst

$(kernel_sys): \
$(OUT)\kernel.ao \
$(OUT)\kernel.co \
$(OUT)\program.co \
$(OUT)\libc\video.co \
$(OUT)\libc\font.co \
$(OUT)\libc\pic.co \
$(OUT)\libc\int.co \
$(OUT)\libc\keyboard.co \
$(OUT)\libc\keycode.co \
$(OUT)\libc\queue.co \
$(OUT)\libc\mouse.co \
$(OUT)\libc\string.co 
	$(LD) -o .out/kernel.sys -N \
	.out/kernel.ao \
	.out/kernel.co \
	.out/program.co \
	.out/libc/video.co \
	.out/libc/font.co \
	.out/libc/pic.co \
	.out/libc/int.co \
	.out/libc/keyboard.co \
	.out/libc/keycode.co \
	.out/libc/queue.co \
	.out/libc/mouse.co \
	.out/libc/string.co


$(OUT)\kernel.ao: kernel.asm
	$(NASM) -felf32 kernel.asm -o $(OUT)\kernel.ao

$(OUT)\kernel.co: kernel.c
	$(CC) kernel.c -o $(OUT)\kernel.co

$(OUT)\libc\video.co: header\video.c
	$(CC) header\video.c -o $(OUT)\libc\video.co

$(OUT)\libc\font.co : header\font.c
	$(CC) header\font.c -o $(OUT)\libc\font.co

$(OUT)\libc\pic.co : header\pic.c
	$(CC) header\pic.c -o $(OUT)\libc\pic.co

$(OUT)\libc\int.co :header\int.c
	$(CC) header\int.c -o $(OUT)\libc\int.co

$(OUT)\libc\queue.co :header\queue.c
	$(CC) header\queue.c -o $(OUT)\libc\queue.co

$(OUT)\libc\keyboard.co :header\keyboard.c
	$(CC) header\keyboard.c -o $(OUT)\libc\keyboard.co

$(OUT)\libc\keycode.co :header\keycode.c
	$(CC) header\keycode.c -o $(OUT)\libc\keycode.co

$(OUT)\libc\mouse.co :header\mouse.c
	$(CC) header\mouse.c -o $(OUT)\libc\mouse.co

$(OUT)\libc\string.co :header\string.c
	$(CC) header\string.c -o $(OUT)\libc\string.co

$(OUT)\program.co : program.c
	$(CC) program.c -o $(OUT)\program.co

kernel.c : \
	header\video.h \
	header\font.h \
	header\asm.h \
	header\keyboard.h \
	header\queue.h \
	header\mouse.h
header\video.c : \
	header\video.h \
	header\font.h \
	header\mouse.h \
	header\asm.h
header\font.c : \
	header\font.h
header\pic.c : \
	header\pic.h \
	header\ioport.h \
	header\asm.h
header\int.c : \
	header\int.h \
	header\asm.h \
	header\video.h 
header\keyboard.c : \
	header\keyboard.h \
	header\pic.h \
	header\queue.h \
	header\asm.h \
	header\ioport.h \
	header\ps2.h
header\keycode.c : \
	header\keyboard.h
header\queue.c : \
	header\queue.h
header\mouse.c : \
	header\mouse.h \
	header\pic.h \
	header\ioport.h \
	header\ps2.h \
	header\queue.h \
	header\video.h \
	header\asm.h \
	header\int.h

header\string.c: \
	header\string.h

program.c: \
	header\font.h \
	header\video.h \
	header\keyboard.h \
	header\string.h 

*.h : header\type.h
*.c : header\type.h

.PHONY: clean
clean:
	-del $(OUT)\*.*  /s /q