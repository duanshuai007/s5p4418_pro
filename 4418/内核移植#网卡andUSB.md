##网卡驱动移植：

从安卓工程中复制驱动源码`drivers/net/ethernet/nexell`到内核源码drivers/net/ethernet/目录中。

修改`drivers/net/ethernet/Kconfig`在文件末尾endif之前添加

```
source "drivers/net/ethernet/nexell/Kconfig"
```

修改`drivers/net/ethernet/Makefile`在文件末尾添加

```
obj-$(CONFIG_NET_VENDOR_NXP) += nexell/
```

make menuconfig

```
Device Drivers->Network device support->Ethernet driver support->
[*]   Nexell devices
<*>     Nexell 10/100/1000 Ethernet driver
[*]       NXPMAC Platform bus support  
```

从安卓工程中复制include/linux/nxpmac.h到内核include/linux/目录下。

编译运行.


##USB驱动：
修改`drivers/usb/Kconfig`
在`config USB_ARCH_HAS_OHCI`和`config USB_ARCH_HAS_EHCI`的配置中分别加入`default y if ARCH_S5P4418`

修改`drivers/usb/host/Kconfig`
在201行,`config USB_EHCI_S5P`选项结束的下方添加一下内容。

```
config USB_EHCI_SYNOPSYS
	boolean "Sysnopsys EHCI support"
	depends on USB_EHCI_HCD && ARCH_CPU_SLSI
	---help---
	  Enable support for the Synopsys's on-chip EHCI controller.

config USB_HSIC_SYNOPSYS
	boolean "Synopsys HSIC support"
	depends on USB_EHCI_SYNOPSYS && ARCH_CPU_SLSI
	---help---
	  Enable support for the Synopsys's on-chip HSIC controller.

config USB_EHCI_SYNOPSYS_RESUME_WORK
	bool "Delay resume for fast resume"
	depends on USB_EHCI_SYNOPSYS && ARCH_CPU_SLSI
		default n
	help
	  Use delay work function to save resume time when resume. but
	  could be lost resume log message, set delay time with platform data
```

在421行，`config USB_OHCI_EXYNOS`选项结束的下方添加内容

```
config USB_OHCI_SYNOPSYS
	boolean "OHCI support for Synopsys"
	depends on USB_OHCI_HCD && ARCH_CPU_SLSI
	help
	 Enable support for the Synopsys's on-chip OHCI controller.
```

在632行,`config USB_HWA_HCD`选项结束的下方添加内容

```
config USB_DWCOTG
	tristate "Synopsis DWC host support"
	depends on USB
	help
	  The Synopsis DWC controller is a dual-role
	  host/peripheral/OTG ("On The Go") USB controllers.

	  Enable this option to support this IP in host controller mode.
	  If unsure, say N.

	  To compile this driver as a module, choose M here: the
	  modules built will be called dwc_otg and dwc_common_port.

config USB_CONNECT_NXP_DRV
	bool "SLsiAP USB connect driver for UBC & OTG"
	depends on USB_DWCOTG
	default n
```

从安卓工程中复制配置文件.config到内核目录中。make menuconfig并保存。

在`drivers/usb/host/ehci-hcd.c`中
在54行附近,在宏`#if defined(CONFIG_PPC_PS3)`结束后的下一行添加

```
#if defined (CONFIG_USB_EHCI_SYNOPSYS)
#include <mach/platform.h>
#endif
```

在1389行附近,在宏`CONFIG_USB_EHCI_HCD_PLATFORM`结束后的下一行添加

```
#ifdef CONFIG_USB_EHCI_SYNOPSYS
#include "ehci-synop.c"
#define PLATFORM_DRIVER		nxp_ehci_driver
#endif
```

复制源文件`ehci-synop.c`到`drivers/usb/host/`目录下
复制源文件`wakelock.h`到`include/linux/`目录下

`make menuconfig`
`Device Drivers->USB support->`
该界面配置以下选项,其他选项默认状态不需要改变。

```
│ │    <*>   EHCI HCD (USB 2.0) support                                          │ │  
│ │    [ ]     Root Hub Transaction Translators (NEW)                            │ │  
│ │    [*]     Improved Transaction Translator scheduling (NEW)                  │ │  
│ │    [*]   Sysnopsys EHCI support                                              │ │  
│ │    [ ]     Synopsys HSIC support (NEW)                                       │ │  
│ │    [*]     Delay resume for fast resume 
```
编译完成。






                                                                                        


