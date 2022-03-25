---
title: Blog_Construction
date: 2022-03-23 10:36:44
index_img: /img/Chinesenewyearlantern.jpg
# banner_img: 
tags:
- 原创
- 博客建设
categories: 
- Technical_Miscellaneous
excerpt: 博客建设  Hexo  GitHub Pages  Node.JS  静态博客网站
---

<!-- <style>
  /* 设置整个页面的字体 */
  html, body, .markdown-body {
    font-family: KaiTi,"Microsoft YaHei",Georgia, sans, serif;
    font-size: 15px;
  }
</style> -->


><font size=5>Hexo + GitHub Pages 创建个人博客网站。</font>

<!-- toc -->

# 一、工具介绍
本文利用了静态博客网站框架 Hexo 和开源托管平台 GitHub 下的 GitHub Pages 来进行基本的静态博客搭建，并将网站部署在 GitHub Pages 上供免费使用。

## 1. hexo
Hexo 是一个快速、简洁且高效的博客框架。Hexo 使用 Markdown（或其他渲染引擎）解析文章，在几秒内，即可利用靓丽的主题生成静态网页。

## 2. GitHub pages
GitHub Pages 可以当作自己或者自己项目的网站，所以 GitHub Pages 有两种最近本的用法：

* 作为你自己（或组织）的网站（访问地址示例： http://username.github.io）
* 作为你某一个项目的网站（访问地址示例： username.github.io/pro....）

# 二、准备工作
接下来就进入搭建个人博客的具体步骤，首先我们需要做一些准备工作。

## 1. 关于 GitHub 账号
我们需要一个 GitHub 账号，没有的话请到 [官网](https://github.com/) 直接申请一个。注册很简单，在这里不做多于赘述。

## 2. 在本地安装 Git
* 下载

网址： https://git-scm.com/download/win  
根据自己的 windows 系统选择对应的 git 版本下载。得到安装包：**Git-2.35.1.2-64-bit.exe**

* 安装

双击安装包开始安装，一路点击 Next 即可，安装完成后鼠标右键会有 Git Bash Here 命令，这是 Git 的命令行，说明安装成功了。  
![Git Bash 界面](./Blog-Construction/GitBash界面.png)

在 bash 命令行中输入如下命令，即可配置好 Git 用户名和邮箱地址（建议与Github保持一致）。

    git config --global user.name "Your Name"
    git config --global user.email "email@example.com"

其余 Git 命令我们将在下面用到时进行说明。

## 3. 安装 Node.JS

由于 Hexo 基于 Node.JS 编写的，所以需要安装 Node.JS 和 npm 工具。

* 下载

网址： https://nodejs.org/zh-cn/download/  
根据自己的 windows 系统选择对应的 git 版本下载。得到安装包： **node-v16.14.2-x64.msi**  
![ Node.JS 下载界面](./Blog-Construction/NodeJS%E4%B8%8B%E8%BD%BD%E7%95%8C%E9%9D%A2.png)

* 安装

双击安装包开始安装，一路点击 Next 即可（自行修改安装路径），安装完成后， msi 格式的安装包会自动添加环境变量。  
![ Node.JS 安装界面](./Blog-Construction/NodeJS%E5%AE%89%E8%A3%85%E7%95%8C%E9%9D%A2.png)

打开 CMD 命令窗口（或者在 Git Bash 下），执行命令 node -v 查看 node 版本。同时也执行 npm -v 查看 npm 版本。  
![ Node.JS Npm 查看版本](./Blog-Construction/%E6%9F%A5%E7%9C%8B%E7%89%88%E6%9C%AC.png)

# 三、创建 GitHub 网站仓库

我们需要在 GitHub 上创建一个仓库，专门用于存放 Hexo 所生成的静态博客文件，并将其推至个人的 GitHub Pages 网站上。

登录 GitHub 官网后，依次点击 头像、下拉菜单的 your repositories 进入仓库页面。  
![ GitHub 仓库页面](./Blog-Construction/GitHub%E4%BB%93%E5%BA%93%E9%A1%B5%E9%9D%A2.png)

点击 New 新建一个崭新的仓库，进入仓库创建页面。在 Repository name 一栏填写仓库名，必须为 <用户名>.github.io，填写完后直接点击 Create repository 创建仓库。  
![ GitHub 仓库创建页面](./Blog-Construction/GitHub%E5%88%9B%E5%BB%BA%E4%BB%93%E5%BA%93%E6%AD%A5%E9%AA%A4.png)

点击 creating a new file 创建一个新文件，作为我们网站的主页。新文件的名字必须为 index.html ，可随便写一点简单内容，示例如下，填写好之后点击 Commit new file 提交。

    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charest="UTF-8">
        <title>Avalon1997</title>
    </head>
    <body>
        <h1>Avalon1997的个人主页</h1>
        <h1>Hello~~~</h1>
    </body>
    </html>

![示例文件代码](./Blog-Construction/%E7%A4%BA%E4%BE%8Bhtml%E6%96%87%E4%BB%B6.png)

在仓库的 Settings 找到 Pages 选项，查看我们个人网站的主页为：https://avalon1997.github.io/  
![个人网页网址](./Blog-Construction/%E6%9F%A5%E7%9C%8B%E7%BD%91%E9%A1%B5%E7%BD%91%E5%9D%80.png)

浏览器中访问页面，打开成功即可。目前的网页及其的简陋，只做展示。

# 四、安装 Hexo

我们采用 Hexo 来创建我们的静态博客网站，使用 Hexo 不需要很大的开发，只进行一些必要的配置即可生成一个个性化的博客网站，非常的方便快捷。

使用 npm 安装 Hexo

    npm install -g hexo-cli

安装好后，查看当前版本

    hexo -v

创建一个项目(例如：MyBlog)并初始化

    hexo init MyBlog
    cd MyBlog
    npm install

本地启动项目测试

    hexo g
    hexo server

浏览器访问网址： http://localhost:4000 ，可查看初始页面风格，在此不配图展示。

# 五、更换主题

Hexo 默认的主题不太好看，喜欢默认主题的请略过此章，需要更换主题的请继续往下看。 
官方提供了数百种主题供用户选择，可以根据个人喜好更换，官网主题请点击 [这里](https://hexo.io/themes/) 查看。本章在这里只介绍个人使用主题 Fluid ，后续章节也是在 Fluid 主题基础上进行讲解。

* 安装 Fluid 主题

下载 [最新 release 版本](https://github.com/fluid-dev/hexo-theme-fluid/tags) 的 rar 压缩包解压到 themes 目录，并将解压出的文件夹重命名为 fluid。

* 指定 Fluid 主题

修改 MyBlog 博客目录下的 _config.yml 文件：

    language: zh-CN
    theme: fluid

* 创建 [关于页]

首次使用主题的 [关于页] 需要手动创建：

    hexo new page about 

创建成功后，编辑博客目录下的 /source/about/index.md ，添加 layout 属性。  
修改后的示例文件如下：

    ---
    title: about
    date: 2022-03-23 18:25:33
    layout: about
    ---

    请在这里写关于页的正文，支持 Markdown、HTML

本地启动测试

    hexo g -d
    hexo s

浏览器访问网址： http://localhost:4000 ，fluid 主题风格如下：  
![Fluid 主题风格](./Blog-Construction/fluid%E4%B8%BB%E9%A2%98%E5%B1%95%E7%A4%BA.png)

# 六、创建文章

如下修改 Hexo 博客目录中的 _config.yml ，打开这个配置是为了在生成文章的同时生成一个同名的资源文件夹用于存放图片文件。

    post_asset_folder: true

执行如下命令创建一篇新文章，名为《测试文章》

    hexo new post 测试文章

执行完后会在 sourec\_post 目录下生成一个md文件和一个同名的资源文件夹，在文件夹中随便放一张图片，然后在 测试文章.md 中添加内容。图片的引入方法可参考 [官方文档](https://hexo.io/zh-cn/docs/asset-folders.html)。

    ---
    title: 测试文章
    date: 2022-03-23 18:34:24
    tags:
    - 原创
    categories:
    - 测试
    ---

    这是一篇测试文章

    {% asset_img test.png 图片引用方法一 %}

    ![图片引用方法二](test.png)

    ![图片引用方法三](/images/test.png)

修改好后本地启动网站

    hexo g -d
    hexo s

访问本地网页即可看到测试文章已经添加。  
![测试创建网站文章](./Blog-Construction/%E6%B5%8B%E8%AF%95%E6%96%87%E7%AB%A0.png)

# 七、个性化页面配置

页面的标题等位置显示默认的文字，可以修改一些个性化的信息。这里的示例仅供参考，其他个性化修改请自行研究 [官方文档](https://hexo.io/zh-cn/docs/)

## 1. 浏览器 tab 页名称

修改根目录下 _config.yml 中的 title 等字段如下。  
![tab页面配置](./Blog-Construction/tab%E9%A1%B5%E9%9D%A2%E9%85%8D%E7%BD%AE.png)

## 2. 博客标题

主题目录 themes\fluid 下 _config.yml 中的 blog_title 字段。  
![博客标题](./Blog-Construction/%E5%8D%9A%E5%AE%A2%E6%A0%87%E9%A2%98.png)

## 3. 主页正中间的文字

主题目录 themes\fluid 下 _config.yml 中的 text 字段。  
![主页中间文字](./Blog-Construction/%E4%B8%BB%E9%A1%B5%E4%B8%AD%E9%97%B4%E6%96%87%E5%AD%97.png)

其他修改不再赘述，修改好后界面如下：  
![个性化修改展示](./Blog-Construction/%E4%B8%AA%E6%80%A7%E5%8C%96%E4%BF%AE%E6%94%B9%E5%B1%95%E7%A4%BA.png)

# 八、发布到 GitHub Pages

安装 hexo-deployer-git 来进行发布。

    npm install hexo-deployer-git --save

修改根目录下的 _config.yml ,配置 GitHub 相关信息如下  
![deployer](./Blog-Construction/%E5%8F%91%E5%B8%83deployer.png)

其中 token 为 GitHub 的 Personal access tokens，获取方式如下图  
![token获取](./Blog-Construction/token.png)

配置好后，执行如下命令自动部署至 GitHub

    hexo g -d

过几分钟以后，刷新自己的 GitHub Pages 页面即可如下图所示  
![网站展示](./Blog-Construction/%E7%BD%91%E7%AB%99%E6%88%90%E5%8A%9F%E5%B1%95%E7%A4%BA.png)

<!-- 组图
{% gi total n1-n2-... %}
  ![](url)
  ![](url)
  ![](url)
  ![](url)
  ![](url)
{% endgi %} -->

><font size=5>至此本文结束，感谢各位看官老爷们观看。</font>































