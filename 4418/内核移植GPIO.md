#gpio移植：

从安卓工程`kernel/drivers/gpio/gpio-nxp.c`复制到内核`drivers/gpio/`目录下
修改`drivers/gpio/Kconfig`，在文件最后添加如下内容

```
config GPIO_NXP
    bool "SLsiAP GPIO"
    depends on ARCH_CPU_SLSI
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_A
    bool "GPIO A (  0 ~  31)"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_B
    bool "GPIO B ( 32 ~  63)"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_C
    bool "GPIO C ( 64 ~  95)"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_D
    bool "GPIO D ( 96 ~ 127)"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_E
    bool "GPIO E (128 ~ 159)"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.

config GPIO_NXP_GROUP_ALV
    bool "GPIO ALIVE (160 ~ )"
    depends on GPIO_NXP
    help
    Select this option to enable GPIO driver for the SLsiAP
    chip family.
```

在`drivers/gpio/Makefile`的最后添加以下内容

```
obj-$(CONFIG_GPIO_NXP)		+= gpio-nxp.o
```

在`make menuconfig`中`Device Drivers->GPIO Support->`

```
 │ │    [*]   SLsiAP GPIO                                                         │ │  
  │ │    [*]     GPIO A (  0 ~  31)                                                │ │  
  │ │    [*]     GPIO B ( 32 ~  63)                                                │ │  
  │ │    [*]     GPIO C ( 64 ~  95)                                                │ │  
  │ │    [*]     GPIO D ( 96 ~ 127)                                                │ │  
  │ │    [*]     GPIO E (128 ~ 159)                                                │ │  
  │ │    [*]     GPIO ALIVE (160 ~ ) 