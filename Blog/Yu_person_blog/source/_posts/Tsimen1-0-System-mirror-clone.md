---
title: Tsimen1.0 System mirror & clone
date: 2022-05-16 16:28:56
index_img: /img/Protocol.jpg
banner_img: /img/Tsimen2.0.jpg
tags:
- 原创
- 计蒙
categories: 
- Tsimen1.0
excerpt: Tsimen1.0 系统镜像/克隆教程
# hide: true
---

>此文章仅用于 Tsimen1.0 镜像系统以及刷机使用。

# <center>镜像 Raspberry 系统</center>

## 一、准备

我们需要做的准备有：  
* 硬件： TF卡、TF读卡器
* 软件： 虚拟机（预装gparted分区软件的linux系统即可），win32磁盘映像工具。

## 二、镜像

1、 插入搭载 TF内存卡的读卡器，连接至虚拟机系统，启动gparted查看U盘：

![插入U盘](./Tsimen1-0-System-mirror-clone/Udisk_2022-05-20_15-10-54.png)

2、 选中 ext4 分区，右键进行分区卸载：

![分区卸载](./Tsimen1-0-System-mirror-clone/Unmount_2022-05-20_15-19-34.png)

3、 更改分区大小，右键此分区选择 resize/move ，将空间扩大 200+ Mb 作为 NewSize ：

{% gi 4 2 %}
![更改分区大小1](./Tsimen1-0-System-mirror-clone/resize_2022-05-20_15-23-42.png)
![更改分区大小2](./Tsimen1-0-System-mirror-clone/Move_2022-05-20_15-25-18.png)
![更改分区大小3](./Tsimen1-0-System-mirror-clone/Expand_2022-05-20_15-48-02.png)
![更改分区大小4](./Tsimen1-0-System-mirror-clone/Confirm_2022-05-20_15-49-04.png)
{% endgi %}

4、 拔出U盘，重新插入电脑连接至主机，新建文件 Raspberry-xx-xx.img ：

![新建img文件](./Tsimen1-0-System-mirror-clone/Mkdirimg_2022-05-20_15-57-46.png)

5、 打开 win32 磁盘映像工具，选择刚刚新建的 .img 文件，选择插入的U盘设备，以仅读取已分配分区读取 TF 卡系统：

![镜像系统](./Tsimen1-0-System-mirror-clone/Readimg_2022-05-20_16-02-44.png)







-----
# <center>克隆 Raspberry 系统 </center>

用于克隆新的 tf 卡，配置新的上位端树莓派系统。

## 一、准备

我们需要做的准备有：  
* 硬件： TF卡、TF读卡器、树莓派
* 软件： win32磁盘映像工具，diskpart分区查看插件，备份好的镜像系统文件。

## 二、克隆

1、 将镜像系统文件准备至自己的PC电脑。

![镜像系统文件](./Tsimen1-0-System-mirror-clone/mirror_2022-05-16_14-33-05.png)

2、 下载并安装 win32系统映像工具

官方下载网址：https://sourceforge.net/projects/win32diskimager/  
![网页界面](./Tsimen1-0-System-mirror-clone/win32mirrorsoftware_2022-05-16_14-35-15.png)

3、 插入搭载 TF内存卡的读卡器，查验内存卡是否为空。如果为空，请直接跳至第4步进行；否则请进行如下清楚操作。

1） 打开cmd命令窗格，输入如下指令。  

    diskpart
    # 回车

2） 在弹出后的diskpart命令窗口中可输入如下指令，  

    list disk                   # 查看目前电脑上的所有磁盘
    select disk 磁盘序号        # 选中对应的磁盘
    list partition              # 查看当前选中磁盘中所有分区
    select partition 分区号     # 选中对应的分区
    delete partition override   #删除选中的分区

<!-- ![此处应有示例图]() -->

4、 写入镜像系统。打开win32磁盘映像工具，选择对应的镜像系统文件tsimen_pi.img以及对应刷机的 tf 卡，点击写入，等待刷机完成。

<!-- ![此处应有示例图]() -->

5、 完成后将 tf 卡插入回树莓派中，启动树莓派。

1） 查看树莓派是否正常运行

    cd tsimen-device/py/data/log
    tail -f 最新日志文件名

    cd tsimen-device/py/data/rar_data/最新的时间文件夹
    ls -l

<!-- ![此处应有示例图]() -->

2） 输入 df -h 查看系统磁盘空间，如果系统磁盘空间偏小，则进行第6步磁盘空间扩容；否则此教程就此结束。

6、 扩容磁盘空间

1） 输入

    sudo raspi-config

2） 在设置界面选择 6 Advanced Options

<!-- ![此处应有示例图]() -->

3） 进入后选择 A1 Expand Filesystem #回车

<!-- ![此处应有示例图]() -->

4） 设置完毕后退出设置界面，重启树莓派后检查磁盘空间，此时将恢复至对应 tf 卡的最大容量。
