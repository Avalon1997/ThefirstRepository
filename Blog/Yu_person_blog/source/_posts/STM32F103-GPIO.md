---
title: STM32F103 GPIO
date: 2022-04-01 17:19:11
index_img: /img/STM32F103_GPIO_1.png
banner_img: /img/STM32F103C8T6_Board.webp
tags:
- 原创
- 单片机
categories: 
- STM32
excerpt: STM32F103VET6 通用 GPIO 介绍
---

> 本片介绍 STM32F103VET6 通用 GPIO 的基本原理，功能及复用。

# 功能描述

每个 GPI/O 端口有两个32位配置寄存器(GPIOx_CRL，GPIOx_CRH)，两个32位数据寄存器(GPIOx_IDR和GPIOx_ODR)，一个32位置位/复位寄存器(GPIOx_BSRR)，一个16位复位寄存器(GPIOx_BRR)和一个32位锁定寄存器(GPIOx_LCKR)。  
在技术手册中列出了每个 I/O 端口的特定硬件特征， GPIO 端口的每个位可以由软件分别配置成多种模式：  
+ 输入浮空  
+ 输入上拉  
+ 输入下拉  
+ 模拟输入  
+ 开漏输出  
+ 推挽式输出  
+ 推挽式复用功能
+ 开漏复用功能
每个 I/O 端口位可以自由编程，然而 I/O 端口寄存器必须按 32 位字被访问（不允许半字或字节访问）。 GPIOx_BSRR 和 GPIOx_BRR 寄存器允许对任何 GPIO 寄存器的读/更改的独立访问，在读和更改访问之间产生 IRQ(Interrupt Request) 不会有任何风险。

![GPIO 端口的基本结构](./STM32F103-GPIO/STM32F103_GPIO_Instruction.png)

![GPIO 端口基本结构（兼容5V）](./STM32F103-GPIO/STM32F103_GPIO_Instruction(compatible%205V).png)





























