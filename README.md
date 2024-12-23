# Information Management System

#### 介绍
CHU简易学生信息管理系统后端

#### 软件架构
后端使用了C++ Crow库

#### 使用说明

本项目在Ubuntu系统中使用cmake进行构建，  
使用了c++ crow库，其头文件直接放在了项目inc路径下，无需进行安装，但是需要安装他的依赖库
```bash
sudo apt-get install libasio-dev
```

使用了sqlite3库，需要进行安装
```bash
sudo apt install sqlite3 libsqlite3-dev
```

使用了nlohmann库，需要进行安装
```bash
sudo apt install nlohmann-json3-dev
```

项目中使用了sqlite数据库来存储各种信息  
先运行build下的init.sql进行数据库初始化
未完......