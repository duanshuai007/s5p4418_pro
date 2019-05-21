##lds链接文件

指定程序代码段的起始位置可以在.ld文件内指定，也可以在Makefile文件中通过-Ttext 0x42c00000指令来指定

通过下面这种方式指定代码段起始地址不能用在测试程序中，只能用在uboot内

$(LD) -g -Ttext 0x70000000 -o walle.elf -e first $(OBJS)

$(CO) -O binary walle.elf walle.bin

下面这种方式指定地址可以用在测试程序中

*.ld

```
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x00000000;
    .text : { 
        main.o
        *(.text)
    }   
    . = ALIGN(4);
    .data : { 
        *(.data)
    }   
    . = ALIGN(4);
    __bss_start = .;
    .bss : { 
        *(.bss)
    }   
    _end = .;
}
```

bss段用来保存未初始化的全局变量，但是实际上只是把未初始化的全局变量所占用的一块空间的首尾地址保存下来，bss段在使用前需要清空。
data段用来存放已初始化的全局变量的一块内存，属于静态内存分配。
text段用来存放程序执行的代码，该区域大小在程序运行前就已经确定，并且该区域通常为只读。在代码段也有可能包含一些只读的常数变量。
rodata段存放C中的字符串和#define定义的常量。
heap堆用来存放进行运行中被动态分配的内存段。大小可变，可动态扩张缩减，用malloc申请堆内存，free释放堆内存。
stack栈用来存放程序临时创建的局部变量，即函数内部定义的变量(不包括static声明的变量，static意味着在数据段中存放变量)。在函数发生调用时，
