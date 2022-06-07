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
excerpt: 查看计蒙运行状态 / 下载光谱数据 / 接线方式 / ...
# hide: true
---

>此文章仅用于 Tsimen1.0 查看系统运行状态、下载数据、接线、查看指示灯等使用。

**<center>说明： 目前Tsimen数据存放文件夹为</center>**

    #最新版本数据存放路径
    home/pi/tsimen-device/py/data

    #宁江设备数据存放路径
    home/pi/.tsimen-device/py/data




# <center>查看计蒙运行状态</center>


1. 点亮树莓派屏幕。

![树莓派屏幕图](./Tsimen1-0-CheckStatus-DownloadFiles/RaspberrySystem_2022-05-16_21-16-45.png)

2. 点击菜单栏终端按钮，打开命令行终端。

{% gi 2 2 %}
![打开命令行终端1](./Tsimen1-0-CheckStatus-DownloadFiles/OpenTerminal_2022-05-17_18-02-42.png)
![打开命令行终端2](./Tsimen1-0-CheckStatus-DownloadFiles/RaspberryTerminal_2022-05-16_21-17-31.png)
{% endgi %}

3. 在弹出的终端窗口中输入：

        #进入日志文件夹
        cd tsimen-device/py/data/log

        #宁江设备请使用如下指令进入日志文件夹
        cd .tsimen-device/py/data/log

![进入日志文件夹](./Tsimen1-0-CheckStatus-DownloadFiles/Log_2022-05-16_21-18-45.png)

4. 检查**当前路径**（蓝色字段即为当前路径）无误后，输入：

        #查看日志文件夹下所有内容
        ls

![ls查看日志文件夹内容](./Tsimen1-0-CheckStatus-DownloadFiles/logls_2022-05-18_15-15-33.png)

5. 记住最新日期的日志文件，输入：

        #实时查看最新的日志文件
        tail -f 日志文件名

![实时查看日志文件](./Tsimen1-0-CheckStatus-DownloadFiles/StrimLog_2022-05-17_17-58-49.png)


# <center>查看上位机（数采仪）程序运行状态</center>


在终端窗口中输入 pgrep python ：如果程序正常运行将显示一串随机整数；否则将  什么都不显示。

        pgrep python

        #显示内容为数字

![查看程序状态](./Tsimen1-0-CheckStatus-DownloadFiles/Python_2022-05-17_18-06-45.png)


# <center>关闭/开启上位机（数采仪）程序</center>


* 关闭：查看到上位机程序状态后，记住显示的整数串，输入如下指令强制关闭程序：

        sudo kill -9 1225
        #1225 为对应现实的整数串

![关闭程序](./Tsimen1-0-CheckStatus-DownloadFiles/KillProgram_2022-05-17_23-00-25.png)

* 开启：输入如下指令开启程序

        sudo python3 tsimen-device/py/run.py &

![开启程序](./Tsimen1-0-CheckStatus-DownloadFiles/Start_2022-05-17_23-02-25.png)



# <center>使用U盘（移动硬盘）拷贝数据</center>

1、 进入数据文件夹，并查看数据文件夹路径(终端窗口请不要关闭）：

    #进入数据文件夹
    cd tsimen-device/py/data/raw_data/

    #查看数据文件夹的路径
    pwd

![查看数据文件夹路径](./Tsimen1-0-CheckStatus-DownloadFiles/DataFold_2022-05-18_17-24-00.png)

2、 请准备一个U盘，将U盘通过USB口插入树莓派：

![插入U盘](./Tsimen1-0-CheckStatus-DownloadFiles/U_2022-05-18_17-26-37.png)

3、 等待半分钟后，重新打开一个新的终端窗口，查看是否接入U盘：

    #进入外设文件夹
    cd /media/pi/
    
    #查看U盘是否接入
    ls

![查看是否接入U盘](./Tsimen1-0-CheckStatus-DownloadFiles/FindU_2022-05-18_17-28-00.png)

4、 如果没有显示U盘名字，请重复2 3 步骤；如果显示U盘名字，进入U盘后新建一个文件夹，自行重命名(例如：时间-地点-Data，2022-05-18-changqing-Data)

    #进入U盘文件夹
    cd #U盘名字

    #新建文件夹（用于存放拷贝数据）
    mkdir #文件夹名字（例：2022-05-18-changqing-Data）

![进入U盘，新建文件夹](./Tsimen1-0-CheckStatus-DownloadFiles/NewFolder_2022-05-18_17-29-35.png)

5、 进入新建的文件夹后，将所需要的数据文件拷贝至此文件夹中：(例如：拷贝2022-05-18-17点的所有数据文件)

    #拷贝文件
    cp -r /home/pi/tsimen-device/py/data/raw_data/2022-05-18/raw_2022-05-18-17* ./

![拷贝文件](./Tsimen1-0-CheckStatus-DownloadFiles/Files_2022-05-18_17-47-11.png)

# <center>计蒙——数采仪段接线颜色及四根线对应的含义</center>

按照如下图所示接线颜色一一对应即可。

![接线对照图](./Tsimen1-0-CheckStatus-DownloadFiles/Status_2022-06-06_16-42-25.png)



# <center>数采仪内部工作指示灯</center>

* RS485模块工作指示灯——共有三个红色指示灯：1 个电源供电指示灯，2 个信号传输指示灯。  
计蒙在正常工作状态下，电源指示灯常亮，两个信号传输指示灯会不间断闪烁。  
如果电源指示灯不亮，电路中其他指示灯正常，说明此485模块损坏。  
如果电源指示灯常亮，两个信号传输指示灯不亮，证明计蒙端没有回传数据，计蒙内部出现异常。

![485指示灯](./Tsimen1-0-CheckStatus-DownloadFiles/Rs485LampChange_2022-05-18_14-13-22.png)

* 树莓派本机工作指示灯——共有一红一绿两个指示灯：红色灯为电源灯，绿色灯为状态指示灯。  
红色灯正常情况下常亮，表示电源正常供电。如果不亮则表明电源供电出现问题（如工作电流太小，USB口挂载大功率设备，树莓派负载功率大等）  
绿色灯正常情况下闪烁，闪烁频率不定，表示正在读取内存卡中内容。如果长时间不闪烁或者上电不闪烁，则可能树莓派系统本身出现问题。

![Raspberry指示灯](./Tsimen1-0-CheckStatus-DownloadFiles/RaspberryLampChange_2022-05-18_14-15-43.png)





# <center>树莓派常用的指令</center>

* ls： 列出当前目录中所有内容（包括文件夹和文件）

![ls/ls -l 查看文件](./Tsimen1-0-CheckStatus-DownloadFiles/ls_2022-05-18_14-49-29.png)

* cd： 跳转至指定目录（更改当前路径至指定目录）

![cd 跳转目录](./Tsimen1-0-CheckStatus-DownloadFiles/cd_2022-05-18_15-02-29.png)

* tail： 查看文件的内容。 使用 tail -f 可以查看正在改变的日志文件（实时查看、实时追踪日志文件）

{% gi 2 2 %}
    ![tail 查看文件](./Tsimen1-0-CheckStatus-DownloadFiles/tail.png)
    ![tail -f 查看文件](./Tsimen1-0-CheckStatus-DownloadFiles/tail-f_2022-05-18_15-10-28.png)
{% endgi %}

* cp： 复制文件或目录到指定路径

![复制文件](./Tsimen1-0-CheckStatus-DownloadFiles/Files_2022-05-18_17-47-11.png)

* mkdir： 在当前目录下新建一个文件夹

![新建文件夹](./Tsimen1-0-CheckStatus-DownloadFiles/Mkdir_2022-05-18_18-07-52.png)


* pwd： 查看当前路径

![查看当前路径](./Tsimen1-0-CheckStatus-DownloadFiles/pwd_2022-05-18_18-09-48.png)













