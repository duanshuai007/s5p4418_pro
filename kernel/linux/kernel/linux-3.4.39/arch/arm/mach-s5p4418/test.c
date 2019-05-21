#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/pgtable.h>
#include <asm/system_misc.h>
#include <asm/hardware/vic.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/cache-l2x0.h>

#include <asm/io.h>
#include <asm/memory.h>

#define readl(x)        (*(volatile unsigned int *)(x))
#define writel(a, v)    (*(volatile unsigned int *)(a) = (v))

#define S5P4418_GPIOC_BASE              0xC001C000
#define GPIO_PULLSEL                    0x58
#define GPIO_PULLENB                    0x60
#define GPIO_PULLSEL_DISABLE_DEFAULT    0x5c
#define GPIO_PULLENB_DISABLE_DEFAULT    0x64
#define GPIO_OUTENB                     0x04
#define GPIO_OUT                        0x00

void mydelay(volatile u32 loop)
{
    for(; loop > 0; loop--);
}

static inline u32 PIN(u8 x)
{
    return 1<<x;
}

void my_printk(void)
{
    printk("hello my printk\r\n");
}

void led_on(int status);
//
//volatile unsigned long virt;
//volatile unsigned long *OUT, *OUTENB, *DETMODE0;
//volatile unsigned long *DETMODE1, INTENB, DET;
//volatile unsigned long *PULLSEL, *PULLENB;
//volatile unsigned long *PULLSEL_DISABLE_DEFAULT, *PULLENB_DISABLE_DEFAULT;
//
//void led_test(void)
//{
//    int val;
//    int i = 100;
//    virt = (unsigned long)ioremap(S5P4418_GPIOC_BASE, 0x100);
//    OUT = (unsigned long *)(virt + 0x00);
//    OUTENB = (unsigned long *)(virt + 0x04);
//    PULLSEL = (unsigned long *)(virt + 0x58);
//    PULLENB = (unsigned long *)(virt + 0x60);
//    PULLSEL_DISABLE_DEFAULT = (unsigned long *)(virt + 0x5c);
//    PULLENB_DISABLE_DEFAULT = (unsigned long *)(virt + 0x64);
//
//    //set pullsel
//    //val = readl( S5P4418_GPIOC_BASE + GPIO_PULLSEL);
//    val = readl( PULLSEL);
//    val |= PIN(1);
//    writel( PULLSEL, val);
//    //set pullenb
//    val = readl( PULLENB);
//    val |= PIN(1);
//    writel( PULLENB, val); 
//    //set pillsel disable
//    val = readl( PULLSEL_DISABLE_DEFAULT);
//    val |= PIN(1);
//    writel(PULLSEL_DISABLE_DEFAULT, val);
//    //set pullenb disable
//    val = readl( PULLENB_DISABLE_DEFAULT);
//    val |= PIN(1);
//    writel( PULLENB_DISABLE_DEFAULT, val);
//    //set dir
//    val = readl( OUTENB);
//    val |= PIN(1);
//    writel( OUTENB, val); 
//
//    while(i)
//    {
//        led_on(0);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        led_on(1);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        mydelay(10000);
//        i--;
//    }
//}
//
//void led_on(int status)
//{
//    int val;
//    if(status)
//    {   
//        val = readl( OUT);
//        val |= PIN(1);
//        writel( OUT, val);
//    }   
//    else
//    {   
//        val = readl(OUT);
//        val &= ~PIN(1);
//        writel( OUT, val);
//    }   
//}

void led_test(void)
{
    int val;
    int i = 100;
    //set pullsel
    //val = readl( S5P4418_GPIOC_BASE + GPIO_PULLSEL);
    val = readl( __phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLSEL));
    val |= PIN(1);
    writel( __phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLSEL), val);
    //set pullenb
    val = readl( __phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLENB));
    val |= PIN(1);
    writel(  __phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLENB), val); 
    //set pillsel disable
    val = readl(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLSEL_DISABLE_DEFAULT));
    val |= PIN(1);
    writel(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLSEL_DISABLE_DEFAULT), val);
    //set pullenb disable
    val = readl(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLENB_DISABLE_DEFAULT));
    val |= PIN(1);
    writel(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_PULLENB_DISABLE_DEFAULT), val);
    //set dir
    val = readl(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUTENB));
    val |= PIN(1);
    writel( __phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUTENB), val); 

    while(i)
    {
        led_on(0);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        led_on(1);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        mydelay(10000);
        i--;
    }
}

void led_on(int status)
{
    int val;
    if(status)
    {   
        val = readl(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUT));
        val |= PIN(1);
        writel(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUT), val);
    }   
    else
    {   
        val = readl(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUT));
        val &= ~PIN(1);
        writel(__phys_to_virt(S5P4418_GPIOC_BASE + GPIO_OUT), val);
    }   
}


