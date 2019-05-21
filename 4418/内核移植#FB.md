##framebuffer驱动移植：
从安卓工程`drivers/video/`目录下复制`nxp-fb.c`文件到内核代码`driver/video/`目录下。

修改`driver/video/Makefile`在文件末尾增加一下内容

``` 
obj-$(CONFIG_FB_NXP)			  += nxp-fb.o
```

修改`driver/video/Kconfig`在第2415行附近，在`config FB_PUV3_UNIGFX`选项结束的地方添加新的选项

```
config FB_NXP
	tristate "SLsiAP framebuffer support"
	depends on FB && ARCH_CPU_SLSI
	select FB_CFB_FILLRECT
	select FB_CFB_COPYAREA
	select FB_CFB_IMAGEBLIT
	---help---
	  Frame buffer driver for the built-in display controller.
	  SLsiAP processor.
	  Say Y here to enable support for the display adaptor.
```

在`drivers/video/`目录下创建文件夹nexell,
在该文件夹下新增Kconfig文件，内容为

```
#
# Video configuration
#

menu "Nexell Graphics"
	depends on HAS_IOMEM

config FB_NXP_ION_MEM
	bool "Allocate framebuffer with ION "
	depends on FB_NXP
	select ION
	select ION_NXP
	default n

config FB0_NXP
	bool "Support primary   frame buffer (/dev/fb0)"
	depends on FB_NXP
	default y

	config FB0_NXP_DISPOUT
		int "Display Out [0/1]"
		depends on FB0_NXP
		default 0

config FB1_NXP
	bool "Support secondary frame buffer (/dev/fb1)"
	depends on FB_NXP
	default n

	config FB1_NXP_DISPOUT
		int "Display Out [0/1]"
		depends on FB1_NXP
		default 1

config NXP_DISPLAY_LCD
	bool "LCD"
	default n

	config NXP_DISPLAY_LCD_IN
		int "Display In  [0=Display 0, 1=Display 1]"
		depends on NXP_DISPLAY_LCD
		default 0

config NXP_DISPLAY_LVDS
	bool "LVDS"
	default n

	config NXP_DISPLAY_LVDS_IN
		int "Display In  [0=Display 0, 1=Display 1]"
		depends on NXP_DISPLAY_LVDS
		default 0

config NXP_DISPLAY_MIPI
	bool "MIPI"

	config NXP_DISPLAY_MIPI_IN
		int "Display In  [0=Display 0, 1=Display 1]"
		depends on NXP_DISPLAY_MIPI
		default 0

config NXP_DISPLAY_TVOUT
	bool "TVOUT"
	default n

config NXP_DISPLAY_HDMI
	bool "HDMI"

	config NXP_DISPLAY_HDMI_IN
		int "Display In  [0=Display 0, 1=Display 1]"
		depends on NXP_DISPLAY_HDMI
		default 0

	choice
		prompt "Resolution"
		depends on NXP_DISPLAY_HDMI
		default NXP_DISPLAY_HDMI_1280_720P

	config	NXP_DISPLAY_HDMI_1280_720P
		bool "1280 *  720p"

	config	NXP_DISPLAY_HDMI_1920_1080P
		bool "1920 * 1080p"

	endchoice

    choice
        prompt "HDMI Audio Interface"
        depends on NXP_OUT_HDMI || NXP_DISPLAY_HDMI
        default NXP_DISPLAY_HDMI_AUDIO_SPDIF
        help
          Choose which interface to use for hdmi audio output

    config NXP_DISPLAY_HDMI_AUDIO_SPDIF
        bool "SPDIF"

    config NXP_DISPLAY_HDMI_AUDIO_I2S
        bool "I2S"

    endchoice

config NXP_DISPLAY_HDMI_USE_HDCP
    depends on NXP_DISPLAY_HDMI
    bool "HDMI use HDCP block"
    default n
    ---help---
      Say y here to enable hdcp module

endmenu
```

在`drivers/video/Kconfig`中的`source "drivers/video/backlight/Kconfig"`的下一行添加以下内容
`source "drivers/video/nexell/Kconfig"`

配置选项`make menuconfig`

`Device Drivers->Graphics support-><*> Support for frame buffer devices ---> <*> SLsiAP framebuffer support`  
`Nexell Graphics--->  [*] LVDS (0)   Display In  [0=Display 0, 1=Display 1] `

选中`System Type` 中的配置项 

```
[*] Support Display SoC  
[*]   Primary   display output (MLC0/DPC0) 
[*]   Secondary display output (MLC1/DPC1) 
[*]   Resolution Converter 
```

编译正常通过，在程序执行时出现
WARNING: at mm/page_alloc.c:2242 __alloc_pages_nodemask+0x1e0/0x7f8()
配置`System Type--->Memory max order (16Mbyte)`编译通过。

此时能够生成fb0设备,并且能够正常运行，但是屏幕上看不到显示的图像，是因为还没有配置背光控制。
配置

```
System Type--->

│ │    [*] PWM driver                                                            │ │  
│ │    [*] PWM driver                                                            │ │  
│ │    [*]   /sys/devices/platform/pwm.N (sysfs interface)                       │ │  
│ │    [*]   pwm 0                                                               │ │  
│ │    [*]   pwm 1                                                               │ │  
│ │    [*]   pwm 2                                                               │ │  
│ │    [*]   pwm 3                                                               │ │  
│ │    -*- Support PWM SoC                                                       
```

配置

```
Device Drivers->Graphics support->Backlight & LCD device support-> Generic PWM based Backlight Driver
```
再次进入开发板，执行测试程序lcd，能够正确的显示图形。


##触摸屏移植(I2C)：
####<PS>在移植触摸屏驱动之前需要先移植gpio驱动。

从安卓工程中复制drivers/i2c/busses/i2c-nxp.c带内核目录的drivers/i2c/busses/目录下。
修改`drivers/i2c/busses/Makefile`
在80行附近增加

```
obj-$(CONFIG_I2C_NXP)       += i2c-nxp.o
obj-$(CONFIG_I2C_SLSI)      += i2c-s3c2410.o
```

修改`drivers/i2c/busses/Kconfig`
在725行附近,在语句`comment "External I2C/SMBus adapter drivers"`的上一行添加以下代码

```
config I2C_SLSI
	tristate "Slsiap I2C"
	depends on ARCH_CPU_SLSI
	help
	  Support for nexell SoC processor's I2C driver.

config I2C_NXP
	tristate "Nxp I2C"
	depends on ARCH_CPU_SLSI
	help
	  Support for nexell SoC processor's I2C driver.

config I2C_NXP_PORT0
	bool "Port 0"
	depends on  I2C_NXP || I2C_SLSI 
		default y

config I2C_NXP_PORT0_GPIO_MODE
	bool "Gpio Mode"
	depends on (I2C_NXP || I2C_SLSI) && I2C_GPIO && I2C_NXP_PORT0
		default n

config I2C_NXP_PORT1
	bool "Port 1"
	depends on I2C_NXP || I2C_SLSI
		default n

config I2C_NXP_PORT1_GPIO_MODE
	bool "Gpio Mode"
	depends on (I2C_NXP || I2C_SLSI) && I2C_GPIO && I2C_NXP_PORT1
		default n

config I2C_NXP_PORT2
	bool "Port 2"
	depends on I2C_NXP || I2C_SLSI
		default n

config I2C_NXP_PORT2_GPIO_MODE
	bool "Gpio Mode"
	depends on (I2C_NXP || I2C_SLSI) && I2C_GPIO && I2C_NXP_PORT2
		default n
```

然后进入配置I2C 

```
[*]   Enable compatibility bits for old user-space (NEW)
<*>   I2C device interface 
[*]   Autoselect pertinent helper modules (NEW) 
	Hardware Bus support->
			<*> GPIO-based bitbanging I2C 
			<*> Nxp I2C 
			[*] Port 0 (NEW)
			[*]   Gpio Mode (NEW)
			[*] Port 1 (NEW)
			[ ]   Gpio Mode (NEW)
			[*] Port 2 (NEW)
			[ ]   Gpio Mode (NEW)
```

增加触摸屏驱动ft5x06驱动模块
复制驱动程序源码，从安卓工程复制`drivers/input/touchscreen/ft5x06_ts.c`和`ft5x06_ts.h`到内核目录`/drivers/input/touchscreen/`下。

修改`drivers/input/touchscreen/Makefile`
在最后一行增加

```
obj-$(CONFIG_TOUCHSCREEN_FT5X0X)       += ft5x06_ts.o
```

修改`drivers/input/touchscreen/Kconfig`
在末尾的endif之前添加

```
config TOUCHSCREEN_FT5X0X
	tristate "FocalTech ft5x0x touchscreen"
	depends on I2C
	default n
	help
	  Say Y here if you have the FocalTech ft5x0x touchscreen.

	  If unsure, say N.

config FT5X0X_MULTITOUCH
	bool "FocalTech ft5x0x touchscreen multi-touch support"
	depends on TOUCHSCREEN_FT5X0X
	default n
	help
	  Say Y here To enable multitouch on Focaltech Touchscreens(required for most).
	  If unsure, say N.
```

然后进入配置项 

```
Device Drivers->input devices->Touchscreens-> <*>FocalTech ft5x0x touchscreen 
```

在make menuconfig中

```
  │ │    -*- Generic input layer (needed for keyboard, mouse, ...)                 │ │  
  │ │    <*>   Support for memoryless force-feedback devices                       │ │  
  │ │    <*>   Polled input device skeleton                                        │ │  
  │ │    < >   Sparse keymap support library                                       │ │  
  │ │          *** Userland interfaces ***                                         │ │  
  │ │    < >   Mouse interface                                                     │ │  
  │ │    < >   Joystick interface                                                  │ │  
  │ │    <*>   Event interface                                                     │ │  
  │ │    < >   Event debugging                                                     │ │  
  │ │          *** Input Device Drivers ***                                        │ │  
  │ │    [ ]   Keyboards  --->                                                     │ │  
  │ │    [ ]   Mice  --->                                                          │ │  
  │ │    [ ]   Joysticks/Gamepads  --->                                            │ │  
  │ │    [ ]   Tablets  --->                                                       │ │  
  │ │    [*]   Touchscreens  --->                                                  │ │  
  │ │    [ ]   Miscellaneous devices  --->                                         │ │  
  │ │        Hardware I/O ports  --->                                              │ │ 
```

选中

```
Device Drivers->Input device support->Touchscreens->FocalTech ft5x0x touchscreen

```
并取消其他触摸屏驱动的选中状态，该操作会在`dev/input/`目录下生成`event[x]`，根据驱动的顺序排号。
经过测试，如果只选中触摸屏驱动，则只会产生一个event，即event0。

测试程序：
这个测试程序必须是在触摸屏和液晶屏都安装驱动后才能测试。
通过`tftp -r libts.tar -g 192.168.200.xxx`将测试程序下载到开发板
解压到`opt/arm/`目录下

进入`opt/arm/lib`目录下，执行`ln -s libts-0.0.so.0.1.1 libts-0.0.so.0`

在开发板根目录执行,event0是制定的触摸屏驱动中断事件

```
export TSLIB_ROOT=/opt/arm
export TSLIB_TSDEVICE=/dev/input/event0
export TSLIB_CALIBFILE=$TSLIB_ROOT/etc/pointercal
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export LD_LIBRARY_PATH=$TSLIB_ROOT/lib
./opt/arm/bin/ts_calibrate
```

如果驱动正常会在液晶屏上弹出测试界面。

也可使用lcd程序进行屏幕显示的测试

