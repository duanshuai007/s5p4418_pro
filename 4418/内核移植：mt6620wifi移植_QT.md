##mt6620移植：

```
1: 	把QT代码中的android/kernel/drivers/misc/mediatek整个文件夹复制到内核代码drivers/misc/目录下。
2: 	把QT代码中的android/kernel/drivers/net/wireless/combo_mt66xx整个文件夹复制到内核代码drivers/net/wireless/目录下。
3: 	把QT代码中的android/kernel/include/linux/combo_mt66xx.h文件复制到内核代码include/linux/目录下。
4: 	把QT代码中的android/kernel/drivers/misc/目录下Kconfig和Makefile文件复制到内核代码/drivers/misc/目录下。
5: 	把QT代码中的android/kernel/drivers/net/wireless/Kconfig和Makefile文件复制到内核代码/drivers/net/wireless/目录下。

6: 	删除driver/misc/Kconfig中的
		source "drivers/misc/haptic/Kconfig"
	删除drivers/net/wireless/Kconfig中的
		source "drivers/net/wireless/bcmdhd/Kconfig"
		source "drivers/net/wireless/DHD_REL_1_141_44_3/Kconfig"
		source "drivers/net/wireless/mt5931/Kconfig"
	删除drivers/net/wireless/Makefile中的
		obj-$(CONFIG_BCMDHD)    += bcmdhd/
		obj-$(CONFIG_BCM43241) += DHD_REL_1_141_44_3/
		obj-$(CONFIG_BCM4354) += DHD_REL_1_141_64/
		obj-$(CONFIG_MTK_MT5931)    += mt5931/
		
7:  从QT代码中复制配置文件config_for_iTOP4418_linux_RTL8211到内核根目录下，并执行cp config_for_iTOP4418_linux_RTL8211 .config
8:	从QT代码中复制android/kernel/include/linux/power_supply.h文件到内核代码include/linux/目录下。
9: 	从QT代码中复制android/kernel/drivers/gpio/gpio-nxp.c文件到内核代码drivers/gpio/目录下。修改drivers/gpio/Makefile，在文件末尾添加obj-$(CONFIG_GPIO_NXP)  += gpio-nxp.o。
10:	make menuconfig 对wifi相关内容进行配置。

    Networking support->Wireless->
	  │ │    --- Wireless                                                              │ │  
	  │ │    <*>   cfg80211 - wireless configuration API                               │ │  
	  │ │    [*]     nl80211 testmode command (NEW)                                    │ │  
	  │ │    [ ]     enable developer warnings (NEW)                                   │ │  
	  │ │    [ ]     cfg80211 regulatory debugging (NEW)                               │ │  
	  │ │    [*]     enable powersave by default (NEW)                                 │ │  
	  │ │    [ ]     cfg80211 DebugFS entries (NEW)                                    │ │  
	  │ │    [ ]     use statically compiled regulatory rules database (NEW)           │ │  
	  │ │    [*]     cfg80211 wireless extensions compatibility (NEW)                  │ │  
	  │ │    [*]   Wireless extensions sysfs files (NEW)                               │ │  
	  │ │    < >   Common routines for IEEE802.11 drivers (NEW)                        │ │  
	  │ │    < >   Generic IEEE 802.11 Networking Stack (mac80211) (NEW) 

	在Devices->Misc Drivers->MTK wireless chip configuration->MediaTek combo_mt66xx Config下的四个模块全部选中为<*>状态
	在Devices->Network device support->Wireless LAN->MTK wireless chip configuration->MediaTek combo_mt66xx WiFi Config->MediaTek Combo Chip Wi-Fi support
	选中为<*>状态
	
	将Networking support->Networking options->Packet socket设置为选中状态
	将Networking support->Networking options->
					[*]   IP: kernel level autoconfiguration
					[*]     IP: DHCP support 
    				[ ]     IP: BOOTP support
					[ ]     IP: RARP support 
					
11:  配置Device Drivers->Input device support->Touchscreens->TSC2007 based touchscreensd，取消选中状态。

12:	电源控制模块的移植
	在迅为开发板上，Wi-Fi模块的电源通过axp22芯片进行控制，所以还需要在内核中加入堆axp22的驱动支持，否则Wi-Fi芯片不上电无法通过tx/rx测试。
	需要注意的是axp22电源控制芯片的驱动是基于i2c驱动的，所以在此之前需要将i2c驱动进行移植。
	在drivers/power/Makefile中追加以下内容
	obj-$(CONFIG_KP_AXP)  += axp_power/
	在drivers/power/Kconfig中加入
	source "drivers/power/axp_power/Kconfig"
	从QT工程中复制代码kernel/drivers/power/axp_power文件夹到内核代码drivers/power目录下。
	在Device Drivers->Power supply class support->AXP Power drivers下的内容全部选中为<*>状态

13:	复制迅为代码中的drivers/mmc/host/dw_mmc.c 和 dw_mmc.h文件到内核代码中，替换原来的dw_mmc.c和dw_mmc.h文件。

14:	从安卓工程中复制include/linux/wakelock.h文件到内核include/linux/目录下。
	删除drivers/mmc/host/dw_mmc.c文件中第40行#include "dw_mmc_dbg.h"。
	修改include/linux/mmc/dw_mmc.h文件内容
	在dw_mci结构体内，在struct mmc_data *data;后添加以下内容
    bool stop_snd;
    struct clk *hclk;
    struct clk *cclk;
    bool prv_err;
	在CONFIG_MMC_DW_IDMAC宏结束的endif	 后添加unsigned int desc_sz;
	在struct work_struct card_work;      后添加struct work_struct resume_work;
	在u32 bus_hz;			后添加u32 max_bus_hz;
	在u32 fifoth_val;		后添加u32 cd_rd_thr;
	在u32 quirks;			后添加struct timer_list timer;

	在make menuconfig中Device Drivers中使能HID设备<*>HID Devices,否则会在编译蓝牙驱动时产生编译错误，undefined reference to `hid_unregister_driver'
	在make menuconfig中使能Device Drivers->MMC/SD/SDIO card support-><*>Support SD/MMC CH 1 （mmc1接口对应Wi-Fi接口的sdio）

15:	驱动移植完成之后，编译stp_uart_launcher-linux-ok.c编译为wmt_launcher，
使用交叉编译工具arm-none-linux-gnueabi-gcc stp_uart_launcher-linux-ok.c -o wmt_launcher
然后将wmt_launcher复制到开发板/usr/bin/目录下。

16: 复制WIFI_RAM_CODE，WMT.cfg，mt6620_patch_e3_0_hdr.bin,
mt6620_patch_e3_1_hdr.bin，mt6620_patch_e3_2_hdr.bin,
mt6620_patch_e3_3_hdr.bin都复制到开发板的/etc/firmware/目录下。

17: 在Kernel hacking内取消Sleep inside atomic section checking的选中状态，
可以使in_atomic不输出调试信息，在ping和wifi驱动中能够用到。如果不关掉该选项，
会导致wifi自检查中的rx超时检测失败。
```

##编译生成wpa工具

```

wpa_suppicant所使用的代码版本为wpa_supplicant-2.5,
对应的openssl版本为0.9.8za(openssl-0.9.8za),
libnl库版本为libnl-1.1.4。  

1.先将wpa_supplicant-2.5/patches/openssl-0.9.8za-tls-extensions.patch
复制到openssl-0.9.8za代码的根目录下,
执行patch -p1 < openssl-0.9.8za-tls-extensions.patch  

2.进入openssl根目录，
创建result文件夹，该文件夹作为最后编译输出的目标文件夹，

修改Makefile，
CC=交叉编译器
AR=交叉编译器-ar$(ARFLAGS) r
RANLIB=交叉编译器-ranlib
INSTALLTOP=刚刚创建的result的绝对路径
OPENSSLDIR=刚刚创建的result的绝对路径

然后执行make和make install

3.进入libnl-1.4.4根目录
执行./configure --host=arm-linux --prefix=/home/xxx/mt6200/libnl1.1.4
prefix后面是指定的生成的链接文件的地址
执行make CC=arm-linux-gcc(需要填写实际的交叉编译工具)
执行make install

4.进入wpa_supplicant-2.5/wpa_supplicant目录，
cp defconfig .config
修改.config增加以下内容
CC=arm-none-linux-gnueabi-gcc -L /home/xxx/mt6620/openssl/openssl-0.9.8za/result/lib
CC是交叉编译链，-L后在第二步中所创建的result目录下的lib地址，LD是交叉编译链的链接器
LD=arm-none-linux-gnueabi-ld

用来指定自己编译的lib库的链接地址,这里是第3步中prefix后的地址
CFLAGS += -I /home/xxx/mt6620/libnl1.1.4/include
LIBS += -L /home/xxx/mt6620/libnl1.1.4/lib

用来指定自己编译的opensll库的链接地址，这里是第1步中创建的result地址
CFLAGS += -I /home/xxx/mt6620/openssl/openssl-0.9.8za/result/include
LIBS += -L /home/xxx/mt6620/openssl/openssl-0.9.8za/result/lib

执行make后会在当前目录下生成wpa_cli,wpa_supplicant,wpa_passphrase三个工具。
将这三个工具复制到开发板上/bin/目录下。
此外还需要将第三部生成的/home/xxx/mt6200/libnl1.1.4/lib/目录下的libnl.so.1
和libnl.so.1.1.4复制到开发板的/lib/目录下.
```

* 增加udhcpc功能支持

```
在etc/目录下创建resolv.conf文件并配置权限chmod 755 resolv.conf
在usr目录下创建文件夹 mkdir -p share/udhcpc/ ,然后将busybox源码根目录下
/examples/udhcp/simple.script复制到开发板/usr/share/udhcpc/目录下，并改名
为default.script,改变其权限chmod 755 default.script。

```

##wpa配置文件

```
在etc目录下新增wpa配置文件wpa_supplicant.conf
其内容为
ctrl_interface=/var/run/wpa_supplicant
eapol_version=1
ap_scan=1
fast_reauth=1
network={
        ssid="FrogsHealth-A908-2G"
        #psk="FrogsHealth@1"
        psk=5c8389548c63a04e11511e4cf4b881b42fc0c1cd84fc47703a5a017692976d29
}
其中第1行是必须的，2-4行为可选项，第5行开始配置了默认网络列表。psk是通过wpa_passphrase ssid psk工具生成的密码。
例子：wpa_passphrase "my_wifi" "my_wifi_password"
```

##将启动过程加入到启动脚本中

```
在etc/init.d/rcS中加入 
/usr/bin/wmt_launcher -m 1 -b 115200 -p /etc/firmware -d /dev/ttyAMA2 &
(其中-p 后的目录是固定不能改变的， -d后的设备号是与Wi-Fi模块所联通的实际的设备号，-b后为波特率，-m后为模式。)
sleep 5
echo 1 > /dev/wmtWifi
ifconfig wlan0 up
sleep 3
wpa_supplicant -i wlan0 -Dnl80211 -c /etc/wpa_supplicant.conf&
sleep 3
/sbin/udhcpc -i wlan0

在drivers/net/wireless/combo_mt66xx/mt6620/wlan/mgmt/ais_fsm.c的1798行函数aisFsmSteps中
在文件开始添加#undef DBG
将#if DBG
    ...
    #else

    #endif
将else中的内容屏蔽。
```

##wpa常用命令

```
wpa工具常用的指令：
1.系统后台服务程序
	wpa_supplicant -i wlan0 -Dnl80211 -c /etc/wpa_supplicant.conf&
2.添加一个新的网络，其中的_n代表_network，是缩写形式。
	wpa_cli add_n
3.设置1号网络ssid为FrogsHealth-A908-2G
	wpa_cli set_network 1 ssid \"FrogsHealth-A908-2G\"
4.设置1号网络密码为FrogsHealth@1
	wpa_cli set_network 1 psk \"FrogsHealth@1\"
5.选择1号网络为连接网络
	wpa_cli select_network 1
6.使能1号网络（同时其他网络被禁止）
	wpa_cli enable_network 1
7.查询连接状态
	wpa_cli status
8.获取wifi网络列表
	wpa_cli list_n
9.扫描周围的网络
	wpa_cli scan
10.显示周围的网络
	wpa_cli scan_r
```

```
关闭mt6620的节能模式(不需要执行)
echo 0 0 > /proc/driver/wmt_dbg
```

##测试脚本

```
/usr/bin/wmt_launcher -m 1 -b 115200 -p /etc/firmware -d /dev/ttyAMA2 &
sleep 5
echo 1 > /dev/wmtWifi
sleep 3
ifconfig wlan0 up
sleep 1
wpa_supplicant -i wlan0 -Dnl80211 -c /etc/wpa_supplicant.conf&
sleep 1
/sbin/udhcpc -i wlan0
执行完以上步骤之后
在shell命令行处输入wpa_cli status
应该返回如下类似消息
		[root@iTOP-4418]# wpa_cli status
		Selected interface 'wlan0'
		bssid=50:fa:84:20:f2:02
		freq=2437
		ssid=FrogsHealth-A908-2G
		id=0
		mode=station
		pairwise_cipher=CCMP
		group_cipher=TKIP
		key_mgmt=WPA2-PSK
		wpa_state=COMPLETED
		ip_address=192.168.200.16
		address=58:12:43:d5:b8:62
说明已经连接到wifi热点。

通过ifconfig会看到出现了wlan0,并且分配了正确的ip。如果udhcpc -i wlan0没能
分配ip，则需要查看usr/share/udhcpc/目录下是否有default.script并查看是否有
可执行权限。

执行ping www.baidu.com -I wlan0能够ping通。
如果执行ping操作出现错误提示：ping: bad address 'www.baidu.com'说明没有设置
正确的路由表.

```