fdemo-a mysql binlog parser.
=====================

Fdemo is a mysql binlog parser written in c++, support mysql5.6 or later.

### note

mysql5.6以后引入了并行复制的概念，但是也是基于库的复制。mysql5.7加入了更细粒度的复制

### usage:
```
1. 进入fdemo目录，执行make，生成二进制文件
2. 配置相应的参数，执行二进制文件
3. 通过日志查看进程的运行情况
```
