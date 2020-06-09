# Oranges

## 简介

编写一个简单的、具备简单系统功能的基于段模式的简单操作系统，并在其上构建了一个简单的打字程序

（大三上学期的课设，完成于2019年12月20日，归档到github于2020年6月9日）

## 项目结构、编译

项目整体采用makefile（mingw64-make）的方式组织，运行于Windows平台。其中编译需要的自制文件（程序）放置在.prebuilt文件夹。编译所需的所有文件如下表：

|程序名称|位置|需要环境|
|-------|----|-------|
|Bochs预配置文件|.\prebuilt\bochs.src|Bochs 2.4.5|
|项目编译脚本|.\prebuilt\compile.ps1|PowerShell|
|linux-dd|.\prebuilt\dd.exe|无|
|FAT16文件写工具|.\prebuilt\fat16\ | Windows10 或 .NET Core3 |
|带MBR的硬盘镜像|.\prebuilt\pure.img|无|
|mingw32-make|MinGW64|
|gcc|MinGW64|
|ld|可以识别纯二进制格式（--oformat binary）的ld链接器，例如wsl下的linux原生ld|
|nasm|NASM2.8|

文件的生成目录为.\\.out\
编译的详细步骤：

1. 在项目根目录运行PowerShell，执行.\\.prebuilt\compile.ps1，传入参数Bochs安装目录。compile.ps1将会根据传入的参数自动在.out文件夹下生成oranges.bxrc供调试使用。然后脚本将会自动调用make命令生成oranges.img
2. makefile脚本编译pbr.asm，生成.\\.out\pbr.bin
3. makefile使用nasm编译kernel.asm为链接文件，使用gcc编译内核文件为链接文件，然后使用ld链接为KERNEL.SYS
4. makefile以.\prebuilt.img为蓝本，使用dd工具拷贝pbr.bin至pbr分区，使用fat16工具将KERNEL.SYS导入到镜像的\SYSTEM\KERNEL.SYS

## 操作系统设计

1. 磁盘镜像

    操作系统镜像为完整的FAT16磁盘镜像，具有被正常磁盘管理软件读取的能力。其中MBR采用Windows（MS-DOS）默认的MBR代码，可以读取分区列表。其中的活动分区则为自己编写的FAT16文件系统的磁盘镜像。活动分区首扇区为FAT16 PBR，分区内部有一个文件SYSTEM\KERNEL.SYS，为操作系统的内核文件

2. PBR模块

    PBR模块负责文件系统的解析、内核文件的读取、32位保护模式切换和操作系统引导。

3. 操作系统内核模块

    操作系统内核具有中断管理、进程管理、文件系统管理等功能。由于时间和经验的不足，目前只完成了中断管理部分。

4. 打字程序

    打字程序可以正常接收从来自操作系统的键盘消息，并作出相应动作，如打字、删除等

## 操作系统实现细节

### 1. 磁盘镜像

程序的磁盘镜像大小为32MB，可以由BIOS进行CHS寻址。第一个扇区MBR的代码属于Windows引导的通用代码，功能即读取MBR尾的分区表，寻找活动分区（0x80），装入内存并启动。磁盘镜像的整体格式可以由任意磁盘工具（或者Linux下的dd命令）生成。磁盘首个分区（也是目前的唯一一个分区）位FAT16格式。

### 2. PBR模块代码解析

由于PBR代码量过大，源代码部分由主代码.\pbr.asm和库代码.\lib\*.asm组成。编译完成的pbr.bin大小超过512字节，所以在源代码出进行了区分。第一部分则为第一个扇区，构成结构如下表：

||
|:---:|
|jmp entry|
|FAT16文件系统格式头：<br/> OEM ID <br/> BPB <br/> 扩展BPB|
|屏幕内容初始化，第二扇区读取，转跳至第二扇区|
|库代码的include宏|
|引导过程所用的字符串|
|PBR尾|

这些内容刚好将第一扇区放满。

第二扇区则为真正的引导逻辑，主要有以下几部分的步骤：

1. 进入SYSTEM文件夹，寻找格式正确的KERNEL.SYS文件
2. 将文件载入至空闲的内存（0x10000~0x80000）
3. 初始化VGA模式
4. 初始化32位保护模式，段选择为平坦模式
5. 重新将内核文件复制到连续的内存空旷部分（0x100000）
6. 将部分重要信息（分区偏移、磁盘ID）保存到寄存器
7. 引导进入操作系统内核

### 3. 操作系统内核模块解析

内核模块主要代码由C语言编写，代码量较多，保守估计有一千行左右。在内核引导部分，代码依然为汇编，即.\kernel.asm。该部分有两小块代码，第一块则是从PBR引导进入的位置（_start标签），负责使用C函数调用约定进入C语言代码执行，第二块代码为中断处理IDT的样板代码，用于C中中断处理代码的衔接。

```x86asm
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
```

_start部分将PBR传来的分区偏移、磁盘ID压如栈，然后call进入 C函数。下一部分为供C中中断初始化程序使用的样板代码。具体内容为将寄存器保存至栈，然后将一个4字节变量也压入栈，然后读取另一个四字节变量，调用该变量。
C主要代码如下

```C

void InitAll()
{
    InitPalette();

    InitAllPIC(); //Unknown bug in mingw-gcc -O3
    InitInterruptTable();
    InitKeyboard();
    //InitMouse();

    DrawBlock(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, videoBuffer, 0xff);
    CopyToVBE(videoBuffer);

    sti();
}

void RunMain();

void OrangesMain(uint32 PartitionOffset, uint32 VolumeSerialNumber)
{

    InitAll();

    RunMain();

    DrawBlock(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, videoBuffer, 0xff);
    DrawString("Program finished.", 0, 0, videoBuffer, 0x1);
    CopyToVBE(videoBuffer);
    while (true)
    {
        hlt();
    }
}

```

进行调色板初始化、8259初始化、中断向量表初始化、键盘初始化、屏幕初始化，然后调用打字程序的RunMain()函数运行。运行完毕后输出program finished，然后循环等待计算机关闭。

### 4. 打字程序解析

打字程序源码为.\program.c

主要内容为while(true)循环调用操作系统提供的函数WaitForKey()，获得从键盘来的按键，然后根据.\header\keycode.c（\header\keyboard.h）中的键盘信息表，处理对应的按键逻辑，显示UI
