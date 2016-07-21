### To-do-list:

1. datetime与datetime2解析格式的不同，还没有判断columnmeta中的值，然后根据这个值往后读几个字节(**DONE**)
2. 拼接新的sql语句的时候对于默认为null的值(如：datetime)要改正，不然生成的sql执行出错
3. 现在只是做了按表来并发，对于热点表的问题要采用按 主键来并发(**DONE**)
4. 了解mysql5.7的并行复制，引入了按区间的概念
5. 现在的线程池的模型是否合适

### 代码目录结构:

- *./binlogevent:* 处理解析之后的binlog，包括按行、按表并发
- *./common:* 线程池相关
- *./slave:* binlog解析相关
- *./utils:* 暂时未用
- *./zk:* 暂时未用

### Attention:
1. mysql5.6之后是对事件做完整性校验的,即每个事件的最后4个字节是checksum，从库在发送dump命令之前，先要通知
   主库，具体看[这里](http://luckybins.blog.51cto.com/786164/1358488).
2. mysql5.6之后datetime的类型的解析发生了变化，具体参见mysql5.6.25的源码，sql/log_event.cc.
