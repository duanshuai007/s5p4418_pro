uboot page_map.h地址映射为nocache，nobuffer需要定义include/configs/s5p4418_drone2.h中的
#######define CONFIG_SYS_ICACHE_OFF
此外还需要将arch/arm/cpu/slsiap/s5p4418/mmu.c中的
#######define SMP_CACHE_COHERENCY
#######define SMP_SCU_ENABE
屏蔽掉。
做了操作就能够启动从nocache，nobuffer空间运行的uboot了。但是启动后运行程序的速度与原来相比非常慢。


cache，就是一种缓存机制，位于CPU和DDR之间，为CPU和DDR之间的读写提供一段内存缓冲区。cache速度比DDR快，但价格贵。
###什么时候不能用cache
#####1 cpu读取外设的内存数据，如果外设数据本身会变化，例如网卡的数据，那么CPU如果连续两次读外设的操作时间很短，而且访问同样的地址，上次的内存数据还在cache中，那么CPU第二次读取的可能还是第一次缓存在cache里的数据。
#####2 CPU外外设写数据，例如向串口控制器的内存空间写数据。如果CPU第一次写的数据还存在于cache中，第二次又向相同地址写数据，CPU可能就更新了一下cache，由cache输出到串口的只有第二次的内容，第一次的内容就丢失了。
#####3 在嵌入式开发环境中，经常需要在PC端使用调试工具来通过直接查看内存的方式以确定某些事件的发生，如果定义一个全局变量来记录中断计数或者task循环次数等，这个变量如果定义为cache的，你会发现有时候系统明明是正常运行的，但是这个全局变量很长时间都不动一下。其实它的累加效果在cache里，因为没有人引用该变量，而长时间不会flush到DDR里。
#####4 考虑双cpu的运行环境(不是双核)。cpu1和cpu2共享一块ddr，它们都能访问,这块共享内存用于处理器之间的通信。cpu1在写完数据到后立刻给cpu2一个中断信号，通知cpu2去读这块内存，如果用cache的方法，cpu1可能把更新的内容只写到cache里，还没有被换出到ddr里，cpu2就已经跑去读，那么读到的并不是期望的数据。
![MacDown Screenshot](/Users/duanshuai/Downloads/1581922-a0a14802a882cac7.png)

	bit[1:0]：映射类型，分段式还是分页式。
		00 忽略
		11 无效，返回Translation fault
		10 分段式
		01 分页式
	nG：0=转换表被标记为全局的;1=转换表属于特定进程;
	S：共享位 0 非共享; 1 共享内存
	XN：0 包含可执行代码; 1 不包含可执行代码
	APX，AP位：权限访问控制位
	Doman：域标识，属于哪个域
	P：ECC校验
	TEX,C,B: 此区域是否采用缓冲buffer，cache，还是直接访问，一般
	外设采用无缓冲，内存采用缓冲方式（个人理解）
	NS：No-Secure 属性
	
发现mmu.c中对映射段段控制位，XN，在第一G空间XN=0，第二G空间XN=1，将第二G空间也修改为XN=0，发现程序能够运行了。

uboot中domain access register = 0x55555555

![MacDown Screenshot](/Users/duanshuai/Downloads/1581922-e7c1f39fcd55788c.png)
共32位16个域，
	
```
00 无权限，任何访问都会产生domain fault
01 Client 检查TLB页表描述符的权限位是否允许访问
10 保留，产生domain fault
11 Manager 不检查访问权限，不会产生权限错误
```
apx| ap | privileged permissions | User permissions
:-:|:-:| :-:| :-:| 
0|b00|No access,recommended use.Read-only when S=1 and R=0 or when S=0 and R=1,deprecated|No access,recommedded use.Read-only when S=0 and R=1,deprecated.
0|b01|R/W| No access.
0|b10|R/W|Read-only
0|b11|R/W|R/W
1|b00|reserved|reserved
1|b01|Read-only| No access
1|b10|Read-only|Read-only
1|b11|Read-only|Read-only
