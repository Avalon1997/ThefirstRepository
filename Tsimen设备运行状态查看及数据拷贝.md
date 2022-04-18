### Tsimen

**<!--数据存放地点及含义-->**

**Tsimen 数据存放文件夹** 

 宁江：~/.tsimen-device/py/data/

目前最新版本：~/tsimen-device/py/data/

![image-20220415134323375](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415134323375.png)

  **/log/** 文件中为 日志文件，软件重启即生成一个以此时刻命名的log文档。其中主要包含，内部树莓派温度及计蒙温度传感器数值、光谱仪每次响应时间及单此响应全谱辐射最大值（dark、ref、samples）；可通过sample一路实时查看当前Tsimen是否被堵。

**/raw_data/**为光谱数据存放文件，软件重启即生成一个以此时刻命名的文件夹。文件夹里存放.json格式文件，每一次采集结束，均会生成一个.json文件，用于存放光谱数据；命名格式为 raw_年-月-日-小时-分钟-秒......

**<!--Tsimen运行状态查看-->**

Tsimen上电后，查看其运行状态。

转到  **/log/** 夹中，

```
cd ~/tsimen-device/py/data/log
# 回车
```

查看最新生成的.log文件

```
ls
# 回车
```

实时查看最新的log文件

```
tail -f tsimen_y-m-d.log
# 回车
```

示例图：

![image-20220415140122901](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415140122901.png)

**<!--Tsimen查看上位机是否运行及重启-->**

查看目前是否运行上位机程序

```
pgrep python
# 回车 
# 假设显示 1111
```

关闭运行

```
sudo kill 1111
# 1111为上一步回车后显示的数字
```

转到程序文件夹

```
cd ~/tsimen-device/py/
# 回车 
sudo python3 run.py &
# 回车 
```

示例图：

![image-20220415140910936](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415140910936.png)

**<!--数据拷贝流程-->**

##### 最新桌面系统

将Tsimen内数据，插U盘拷贝

```
# 找到u盘
cd /media/pi/
# 回车 
ls
# 回车 查看是否有插入的u盘
cd u盘名字
# 回车 
mkdir test
# 回车  新建一个文件夹 test为文件夹名称(用于存放要拷贝出来的数据)
cd test
# 回车 跳转到新建文件夹
'''以需要拷贝2021-11-19-23-10分-20分的全部.json数据到当前路径'''
cp -r ~/tsimen-device/py/data/raw_data/2021-11-19/raw_2021-11-19-23-* .
# 回车 cp -r 需要拷贝的文件路径 存放拷贝文件路径
ls
# 回车 可查看 是否拷贝成功
```

示例图：

![image-20220415151311887](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415151311887.png)



##### 通过链接网线或热点，基于ssh进行拷贝

将Tsimen内数据，拷贝到新设备上

（前面对于u盘位置的寻找以及新建文件夹的方法，不再赘述）

**Tsimen端**

```
ifconfig
# 回车 查看ip地址
```

示例图

![image-20220415151850476](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415151850476.png)

****

**接收端**

```
'''以需要拷贝2022-04-08-13-20分-30分的全部.json数据到当前路径'''
scp -r "pi@192.168.5.15:~/tsimen-device/py/data/raw_data/2021-11-19/raw_2021-11-19-23-*" .
# scp -r "设备名@ip地址:需要拷贝的文件路径" 存放拷贝文件路径
# 回车 输入密码 /为保护隐私，输入密码不会显示，不要以为是没有输入
raspberry
# 回车
```

示例图：

![image-20220415152744565](C:\Users\Admin\AppData\Roaming\Typora\typora-user-images\image-20220415152744565.png)