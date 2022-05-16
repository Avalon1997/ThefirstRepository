---
title: Tsimen1.0 CheckStatus & DownloadFiles
date: 2022-05-16 17:02:39
index_img: /img/Protocol.jpg
banner_img: /img/Tsimen2.0.jpg
tags:
- 原创
- 计蒙
categories: 
- Tsimen1.0
excerpt: 查看计蒙运行状态 / 下载光谱数据
# hide: true
---

>此文章仅用于 Tsimen1.0 查看系统运行状态，下载数据使用。

**<center>说明： 目前Tsimen数据存放文件夹为</center>**

    #最新版本数据存放路径
    home/pi/tsimen-device/py/data

    #宁江设备数据存放路径
    home/pi/.tsimen-device/py/data



# <center>查看计蒙运行状态</center>
-----

1. 点亮树莓派屏幕。

![树莓派屏幕图]()

2. 点击菜单栏终端按钮，打开命令行终端。

![打开命令行终端]()

3. 在弹出的终端窗口中输入：

        #进入日志文件夹
        cd tsimen-device/py/data/log

        #宁江设备请使用如下指令进入日志文件夹
        cd .tsimen-device/py/data/log

![进入日志文件夹]()

4. 检查**当前路径**（蓝色字段即为当前路径）无误后，输入：

        #查看日志文件夹下所有内容
        ls

5. 记住最新日期的日志文件，输入：

        #实时查看最新的日志文件
        tail -f 日志文件名

![实时查看日志文件]()
















