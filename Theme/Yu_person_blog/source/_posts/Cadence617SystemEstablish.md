---
title: Cadence617SystemEstablish
date: 2022-05-13 21:56:32
# index_img: /img/microcontroller.jpeg
# banner_img: /img/1024px-STM32F103VGT6-HD.jpg
tags:
- 原创
- IC Design System
categories: 
- IC Design
excerpt: 模拟IC集成电路设计系统搭建教学。
hide: true
---




# 二、下载运行库

在安装设计软件前，我们需要在linux系统内下载各个软件正常运行需要的配置库。

    //所有软件运行需要的配置库
    yum update kernel -y
    yum install kernel-headers -y
    yum install kernel-devel -y
    yum install gcc -y
    yum install make -y
    yum install ksh -y
    yum install libXext.so.6 -y
    yum install libXtst.so.6 -y
    yum install libXt.so.6 -y
    yum install libGLU.so.1 -y
    yum install mesa-libGLU -y
    yum install libelf.so.1 -y
    yum install libXrender.so.1 -y
    yum install libXp.so.6 -y
    yum install libXrandr.so.2 -y
    yum install *xorg* -y
    yum install libXp -y
    yum install ld-linux.so.2 -y
    yum install openmotif -y
    yum install libstdc++.so.5 -y
    yum install xterm -y
    yum install gcc -y
    yum install gcc-c++ -y
    yum install csh -y
    yum install *xorg-X11-fonts* -y
    yum install compat-libstdc++-33.i686 -y
    yum install alliance-libs -y
    yum install alliance -y
    yum install glibc-2.12-1.107.el6.i686 -y
    yum install glibc-devel.i686 -y
    yum install glibc -y
    yum install compat-readline5-5.2-17.1.el6.i686 -y
    yum install xterm-253-1 -y

    //建立三个软链接，报错请直接忽略
    ln -s /usr/bin/awk /bin/awk
    ln -s /usr/lib/libstdc++-3-libc6.2-2-2.10.0.so /usr/lib/libstdc++-libc6.1-1.so.2
    ln -s /bin/gunzip /bin/uncompress

    //以下两个语句请连续执行，中间不得分开
    cd /lib64
    ln -s libreadline.so.6 libreadline.so.5

    //以下两个语句请连续执行，中间不得分开
    cd /usr/lib64
    ln -s libhistory.so.6 libhistory.so.5



