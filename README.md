# FPTreeDB键值存储系统

[TOC]

## 背景介绍

### 概括

在进行NVM相关实验和FPTree论文的阅读后，对于FPTree以及NVM编程以及拥有初步的了解。

在数据库的学习过程中，对B+tree有所学习了解，FPTree的结构与普通B+tree是类似的，存放在DRAM的中间及节点管理方式与普通B+tree一样。本次课程基于针对NVM优化的数据结构FPTree，实现一个简单的键值存储引擎FPTreeDB。

### 论文

FPTree论文中提出了一种新的混合SCM-DRAM持久并发B树,即FPTree,它与基于DRAM的并行B树具有相似的性能,介绍了FPTree的详细结构和具体的操作,可以保证从任意点的故障恢复到一个一致性的状态而不丢失信息,除此之外
FPTree的访问速度能够与DRAM相当,对不同的SCM设备的延迟有很好的弹性,同时也能支持高并发的场景。目的是
改进传统的应用在主存的B-Tree索引结构,因为作者认为传统的索引技术没能满足新硬件特性下的一致性要求,例如
对比于DRAM,SCM的延迟更高,写速度比读速度慢很多,除此之外,设计持久化的数据结构、保证数据一致性很
难,因为SCM是通过易失的CPU cache进行访问的,软件对于此的控制力很小。简而言之,如果需要更好的性能,就
需要设计更适合的数据结构,于是作者设计了FPTree索引,希望可以达到能够相当于DRAM的性能,同时解决一些关
于持久化内存泄漏和数据恢复的问题。

### 前期准备

1. 参考Intel的NVM模拟教程模拟NVM环境,用fio等工具测试模拟NVM的性能并与磁盘对比，发现NVM性能远远优于磁盘。

2. 使用PMDK的libpmem库编写样例程序操作模拟NVM，lipbmem是PMDK库中最基本的包之一。

3. 持久化指针，SCM硬件等内容的了解。

   

------

## 项目介绍

### 运行环境

​	Ubuntu 18.04.2 LTS,64bit

### 项目概述

该系统基于针对NVM优化的数据结构FPTree，实现一个简单的键值存储引擎。通过将其包装成一个调用库，供用户程序使用并管理其数据村村，与LevelDB的使用方法类似。其对外可用的对数据的基本操作为增删改查以及系统恢复。

### 项目结构

![FPTreeDB](/asset/FPTreeDB.png)

### 项目目录说明

```
|__gtest: Google Test项目目录 
|__include: 包含所有用到的头文件  
   |__fptree: fptree的头文件所在文件夹  
      |__fptree.h: fptree地头文件  
   |__utility: fptree所用工具的头文件所在文件夹  
      |__utility.h: 指纹计算等工具函数所在头文件  
      |__clhash.h: 指纹计算所用哈希函数头文件  
      |__p_allocator.h: NVM内存分配器头文件  
|__src: 项目源码所在地
   |__bin: 可执行文件所在文件夹
      |__main: main.cpp的可执行文件
      |__lycsb: lycsb.cpp的可执行文件
      |__ycsb: ycsb.cpp的可执行文件
   |__fptree.cpp: fptree的源文件，项目核心文件  
   |__clhash.c: 指纹计算的哈希函数源文件  
   |__p_allocator.cpp: NVM内存分配器源文件
   |__lycsb.cpp: LevelDB的YCSB测试代码
   |__ycsb.cpp: FPTreeDB和LevelDB的YCSB对比测试代码  
   |__makefile: src下项目的编译文件  
|__workloads: 为YCSB测试负载文件，用于YCSB Benchmark测试  
   |__数据量-rw-读比例-写比例-load.txt: YCSB测试数据库装载文件  
   |__数据量-rw-读比例-写比例-run.txt: YCSB测试运行文件  
|__test: 为Google Test用户测试代码所在，请完成编译并通过所有测试  
   |__bin: 单元测试可执行文件所在文件夹
      |__fptree_test: fptree_test.cpp的可执行文件
      |__utility_test: utility_test.cpp的可执行文件
   |__fptree_test.cpp: fptree相关测试  
   |__utility_test.cpp: PAllocator等相关测试  
   |__makefile: gtest单元测试的编译文件   
```

### 时间计划

1. 系统说明书，PAllocator实现并通过utility测试，LevelDB的使用以及测试，对应lycsb.cpp，p_allocator.cpp的实现和运行，utility_test.cpp的运行 --- 5/4晚前发布v1版本branch。
2. FPTreeDB插入和重载操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行 --- 5/11晚前发布v2版本branch。
3. )FPTreeDB查询和更新操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行 --- 5/18晚前发布v3版本branch
4. FPTreeDB删除操作和所有剩下实现以及测试，对应fptree.cpp的实现和fptree_test.cpp所有的运行 --- 5/31晚前发布final版本branch，作为最后发布版本

------

## 使用说明

1. 编译安装PMDK库和lipbmem库

   ```
   git clone https://github.com/pmem/pmdk.git
   make
   make install
   sudo apt-get install libpmem-dev
   ```

2. 编译安装LevelDB

   ```
   git clone https://github.com/google/level.git
   mkdir -p build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
   make install
   ```


3. 编译main和其他代码

