#s5p4418 memory map
#####Data Bus Width:32-bit
SYSRSTCONFIG = 0xC001_0000

##Memory Map

| Memory | Address | Size|
|:-------------:	|	:-------:		|:--------:|
|Internal SRAM	|	0xFFF00000	| 	|
| Reserved    	| 	0xE0000000	| 	|
|Normal I/O	 	| 	0xC0000000	| 	|
|MCU-A(2GB)	 	| 	0x40000000	| 	|
|MCU-S STATIC		|	0x00000000	|	|

##MCU-S Static 

|MCU-S Static	| Address 	| Length 	|	
|:----------:	|:--------:	| :-----: |
|Reserver		|0x34005000	| 保留空间 	|
|InternalROM	|0x34000000	| 20KB 	|
|Reserver		|0x30000000	| 保留空间	|
|NAND			|0x2C000000	| 1MB		|
|Reserver		|0x08000000	| 保留空间 	|
|Static#1		|0x04000000	| 1MB		|
|Static#0		|0x00000000	| 1MB		|
###MCU-S Bank
  用来连接普通静态内存设备(SRAM/ROM)或NAND，最多有两个静态片选信号存在。
####NAND Flash Controller
  提供小块和大块的NAND flash内存。最多连接两个NAND flash内存。
####InternalRom
该部分内存包括20KB的空间，在这部分空间内有一个特殊的程序。在复位后，cpu从InternalROM的0地址处执行指令。InternalROM有一个代码提供不同的Booting方法。该方法被定义为internal ROM Booting（iROMBOOT）.
iROMBOOT使用internal SRAM来存储栈或数据，因此iROMBOOT在执行后可能会改变internal SRAM的内容。
iROMBOOT提供五种Booting modes：USBBOOT，UART BOOT, SPI Serial EEPROM BOOT,SDHCBOOT,NANDBOOT带有错误校验。

#U-Boot page_map.h

virt | phys | size(MB) ||
:---:| :---:| :---:|:---:|
0x40000000 | 0x40000000 | 1024 | DDR0-1G|
0x80000000 | 0x80000000 | 1024 | DDR1-2G|
0xC0000000 | 0xC0000000 | 3 | NORMAL IO|
0xF0000000 | 0xF0000000 | 1 | SCU |
0xFFF00000 | 0xFFF00000 | 1 | SRAM |
0xCF000000 | 0xCF000000 | 1 | L2 cache |
0x2C000000 | 0x2C000000 | 1 | NAND |
0x04000000 | 0x04000000 | 1 | CS1 |
0x00000000 | 0x00000000 | 1 | CS0 |
0|0|0|PAGE_END|

在uboot中将页表保存在0x42c80000位置处，将该地址传给enable_mmu函数写入mmu配置寄存器中。仅仅使能了0x80000000-0xC000000地址空间的Cachable和Bufferable.在mmu_asm.S中enable_mmu开启了Icache和Dcache.

在uboot中.text代码段的起始地址由__image_copy_start可以看出时0x42C00000.在include/configs/s5p4418_drone2.h文件中由CONFIG_SYS_TEXT_BASE进行定义。
在u-boot.lds中可以看出.text,.rodata,.data,.u_boot_list属于image_copy_start到image_copy_end范围之间，然后紧接着的就是rel_dyn_start到rel_dyn_end.接下来时mmutable。最后是bss段内容。

#kernel

在linux内核中arch/arm/kernel/vmlinux.lds文件中指定了内核的起始地址为0xC0000000 + 0x00008000.
在arch/arm/mach-s5p4418/include/mach/s5p4418_iomap.h中定义了内核中的mmu地址映射表。

 *  Length must be aligned 1MB 
 *  Refer to mach/iomap.h
 *  Physical: 	\__PB\_IO\_MAP\_ ## \_n\_ ## \_PHYS
 *  Virtual: 		\__PB\_IO\_MAP\_ ## \_n\_ ## \_VIRT
 *  Parmeters: .name .virtual, .pfn, .length, .type
 
| macro 	|	...  |
|:-: 		| :---:|
| PB\_IO\_MAP(REGS, 0xF0000000, 0xC0000000, 0x00300000, MT_DEVICE )|  NOMAL IO, Reserved|
| PB\_IO\_MAP(MPPR, 0xF0300000, 0xF0000000, 0x00100000, MT_DEVICE )| Cortex-A9 MPcore Private Memory Region (SCU) |
|PB\_IO\_MAP(L2C, 0xF0400000, 0xCF000000, 0x00100000, MT_DEVICE )| PL310 L2 Cache |
|PB\_IO\_MAP(NAND, 0xF0500000, 0x2C000000, 0x00100000, MT_DEVICE )| NAND|
|PB\_IO\_MAP(IROM, 0xF0600000, 0x00000000, 0x00100000, MT_DEVICE )| IROM|

在表中使用宏PB\_IO\_MAP，该宏有两处定义。

在arch/arm/mach-s5p4418/include/mach/iomap.h
#######define PB\_IO\_MAP(\_n\_, \_v\_, \_p\_, \_s\_, \_t\_)
	enum {                                      \
        __PB_IO_MAP_ ## _n_ ## _VIRT    = _v_,  \
        __PB_IO_MAP_ ## _n_ ## _PHYS    = _p_,  \
        };

* 以下这段代码并没有被s5p4418_iomap.h调用        

在arch/arm/mach-s5p4418/include/mach/map_desc.h中也定义了
#######define PB\_IO\_MAP(\_n\_, \_v\_, \_p\_, \_s\_, \_t\_)
    {                                       
        .virtual    = _v_,                  
        .pfn        = __phys_to_pfn(_p_),   
        .length     = _s_,                  
        .type       = _t_                   
    },
在arch/arm/include/asm/memory.h中定义了

	#define __phys_to_pfn(paddr)
	    ((unsigned long)((paddr) >> PAGE_SHIFT))
	#define __pfn_to_phys(pfn)
	  ((phys_addr_t)(pfn) << PAGE_SHIFT)
 

对于内存管理，linux在启动过程中分为两个阶段，第一阶段使用memblock管理内存，用于内核启动过程。第二阶段内核启动后由buddy内存管理来接手内存。
在启动信息中有一段Virtual kernel memory layout:

| name |address|size|..|
|:----:|:-----:|:--:|:--|
|vector| 0xffff0000 - 0xffff1000 | 4KB |映射cpu的中断向量表，中断产生时，pc指针会自动跳转到0xffff0000+4*vector_num的地方。|
|fixmap| 0xfff00000 - 0xfffe0000 | 896KB |该区域属于最顶部的pte页表，为系统中对每个cpu保留16个page页对虚拟地址。用来将高端物理内存页映射到内核的线性地址范围，使内核能够访问，对应api是kmap_atomic/kummap_atomic该函数是原子操作，不会休眠。使用该函数处理完后应尽快调用kunmap_atomic进行释放。可以用于中断上下文。|
| dma | 0xfee00000-0xffe00000 | 16MB| 该区域的开始地址和大小在arch/arm/mm/dma-mapping.c中指定。分别由consistent_base，DEFAULT_CONSISTENT_DMA_SIZE，CONSISTENT_END指定该区域的开始地址，大小，结束地址。该区域的内存分配api函数为：dma_alloc_coherent/dma_free_coherent，该分配函数会建立映射表，并且分配出来的物理地址是连续的。dma_alloc_coherent的核心函数为：__dma_alloc。具体详细的流程，请见我的另外一篇blog。在调用这个api进行dma内存分配时，虚拟地址是从CONSISTENT_END高地址往consistent_base低地址方向分配的，即第一次dma_alloc_coherent调用的返回值>第二次dma_alloc_coherent调用的返回值。
|vmalloc| 0xef800000 - 0xfee00000 | 246MB |高端内存，该区域虚拟地址是连续的，但是对应的物理地址可能是不连续的。对应的api是vmalloc和vfree。可以用来分配大块内存。|
|lowmem| 0xc0000000 - 0xef600000 | 758MB |内核地址空间的直接映射区，即内核的低端内存区。与ram内存物理地址空间中对应的地址只差一个固定对偏移量。这里偏移量是0x80000000。|
|pkmap | 0xbfe00000 - 0xc0000000 | 2MB |用来将高端物理内存页映射到内核的线性地址范围，使内核能够访问，对应api是kmap/kunmap，该函数能够休眠，在地址资源紧张时就会发生休眠。|
|modules| 0xbf000000 - 0xbfe00000 | 14MB |为内核模块分配地址空间|
|.text| 0xc0008000 - 0xc0944ac4 | 9459KB |代码段|
|.init| 0xc0945000 - 0xc097e000 | 228KB |启动过程中的数据，启动完成后清空。
|.data| 0xc097e000 - 0xc0a01a88 | 527KB |初始化数据段|
|.bss| 0xc0a01ac - 0xc0c26140 | 2194KB|全局变量未初始化数据段|
