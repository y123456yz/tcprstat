
/*
参考Lowercases的tcprstat做了修改，新增了慢响应个数统计，时间戳、慢响应时间日志记录，有利于快速定位慢响应数据报文，从而分析系统中可能存在的潜在问题。  
  
install
===================================  
centos 6.5  
yum install perf  
yum -y install bison yacc  
yum -y install flex  
yum install patch  
yum install glibc-static  
autoreconf -fvi  
./configure  
make & make install  
  

  
命令行参数:
===================================  
-p:端口
-l:ip
-o:打印时延超过-T参数指定的包的数量
-T:时延统计阈值，超过该时间的会通过tc打印超过该时延阈值的包个数
-o:如果有包的时延超过阈值，则把该包的时间戳记录到该文件中，这样可以帮助分析报文
-t:统计打印时间间隔
-n:最多打印这么多次
-r:离线统计功能的抓包文件
  
  
  
示例：
===================================  
示例一：在线时延实时统计  
-----------------------------------   
下面是统计10.12.1.1服务器上面，端口为12322的数据包的响应时间：  
-t 1 表示每隔1s统计一次，也就是每行的打印  
-T 1 表示时延超过1ms的请求的个数。  
-o tcprstat.log  把时延超过-T设置的1ms的数据包时间戳记录到文件，并记录该包的响应时间，见后面的时延tcprstat.log内容  
  
[root@s10-12-1-1 test]# ./tcprstat  -l 10.12.1.1 -p 12322 -n 3333 -t 1 -T 1 -o tcprstat.log  
timestamp       count   max     min     avg     med     stddev  tc      95_max  95_avg  95_std  99_max  99_avg  99_std  
1483946797      4002    1466    42      199     194     106     6       359     186     82      465     194     92  
1483946798      3498    1456    44      208     211     103     1       364     196     87      463     204     94  
1483946799      3485    1262    40      203     200     103     1       371     190     87      479     199     96  
1483946800      3867    2499    38      206     202     116     5       382     191     86      503     201     96  
1483946801      3992    1973    41      201     197     114     6       362     186     84      499     195     94  
1483946802      4271    1828    39      196     193     102     1       362     183     83      476     192     91  
1483946803      3591    1678    42      206     202     110     5       374     192     86      511     201     94  
1483946804      4510    1889    32      195     190     101     2       351     183     78      457     191     87  
1483946805      4580    1877    42      192     185     100     3       349     179     78      460     188     86  
1483946806      4386    1999    37      192     187     105     4       352     179     83      462     187     91  
1483946807      4885    1821    32      183     180     97      1       333     171     80      421     179     87  
1483946808      4898    1898    40      193     192     98      2       344     180     80      446     189     86  
1483946809      4208    2015    41      197     194     105     2       357     185     81      462     193     90  
1483946810      3970    1101    42      196     192     96      2       353     184     79      448     192     88  
1483946811      4146    1252    41      197     193     100     2       353     184     82      456     192     90  
1483946812      3846    1024    41      201     197     101     2       361     189     82      456     197     91  
1483946813      3717    1428    42      207     206     101     4       369     195     83      457     203     92  
1483946814      3933    1534    43      199     194     102     3       353     186     82      446     194     90  
1483946815      4014    1625    37      199     198     98      1       352     187     81      446     195     89  
1483946816      3923    752     38      196     194     97      0       353     184     84      456     193     91  
1483946817      3993    2116    42      201     199     102     4       354     188     83      449     196     90  
1483946818      4005    1641    44      201     199     101     1       358     189     82      462     197     91  
1483946819      4246    1549    42      195     187     101     3       354     183     81      442     191     89    
    
tcprstat.log记录慢响应数据包的时间戳，delay_time单位为us：    
[root@s10-12-1-1 test]# cat tcprstat.log   
timestamp:     1483946796.658649     delay_time:           1136(该数据报文产生时间戳是1483946796.658649，响应时间戳是1483946796.658649 + 1136us)  
timestamp:     1483946796.884682     delay_time:           1292      
timestamp:     1483946796.954704     delay_time:           1326       
timestamp:     1483946797.327451     delay_time:           1399    
timestamp:     1483946797.437754     delay_time:           1085    
timestamp:     1483946797.446523     delay_time:           1466    
timestamp:     1483946797.804099     delay_time:           1456    
timestamp:     1483946799.176803     delay_time:           1262  
timestamp:     1483946799.643757     delay_time:           2499  
timestamp:     1483946800.97223      delay_time:           1151  
timestamp:     1483946800.103771     delay_time:           1164  
timestamp:     1483946800.120869     delay_time:           1315  
timestamp:     1483946800.388201     delay_time:           1557  
timestamp:     1483946800.655855     delay_time:           1191  
timestamp:     1483946800.872843     delay_time:           1153  
timestamp:     1483946800.963374     delay_time:           1871  
timestamp:     1483946801.9984       delay_time:           1973  
timestamp:     1483946801.152817     delay_time:           1951  
timestamp:     1483946801.181594     delay_time:           1376  
timestamp:     1483946801.852420     delay_time:           1828  
timestamp:     1483946802.627323     delay_time:           1037  
timestamp:     1483946802.857785     delay_time:           1134  
timestamp:     1483946803.22034      delay_time:           1546  
timestamp:     1483946803.243553     delay_time:           1678  
timestamp:     1483946803.255998     delay_time:           1299  
timestamp:     1483946803.672670     delay_time:           1889  
timestamp:     1483946804.297324     delay_time:           1062  
timestamp:     1483946804.779147     delay_time:           1306  
timestamp:     1483946805.76330      delay_time:           1877  
timestamp:     1483946805.398714     delay_time:           1030  
timestamp:     1483946805.599980     delay_time:           1036  
timestamp:     1483946805.834343     delay_time:           1999  
timestamp:     1483946806.332698     delay_time:           1352  
timestamp:     1483946806.543452     delay_time:           1447  
timestamp:     1483946807.260249     delay_time:           1821  
    
  
     
示例二：离线分析，通过wireshark工具查看时间戳就可以确定出抓包文件中慢响应报文的时间戳，就可以快速定位慢响应报文，分析响应慢的原因  
-----------------------------------  
[root@s10-12-1-1 test]# tcpdump -i bond0 port 15211 -w  test.pcap  
tcpdump: listening on bond0, link-type EN10MB (Ethernet), capture size 65535 bytes  
^C90800 packets captured  
91141 packets received by filter  
291 packets dropped by kernel  
[root@s10-12-1-11 test]#   
[root@s10-12-1-11 test]# ./tcprstat  -l 10.12.1.1 -p 15211 -n 3333 -t 1 -T 1 -o tcprstat.log -r test.pcap   
timestamp       count   max     min     avg     med     stddev  tc      95_max  95_avg  95_std  99_max  99_avg  99_std  
1483947284      5041    1943    38      181     180     97      2       334     169     79      441     177     87  
1483947285      4957    1677    39      184     185     98      3       330     172     79      440     180     86  
1483947286      4799    1147    38      189     190     97      1       345     177     81      449     185     89  
1483947287      4982    1317    39      185     182     96      2       338     173     82      417     181     88  
[root@s10-12-1-1 test]#   
[root@s10-12-1-1 test]# cat tcprstat.log   
timestamp:     1483947283.777246     delay_time:           1943  
timestamp:     1483947284.94361      delay_time:           1015  
timestamp:     1483947284.848973     delay_time:           1216  
timestamp:     1483947285.397089     delay_time:           1187  
timestamp:     1483947285.438686     delay_time:           1677  
timestamp:     1483947286.702593     delay_time:           1147  
timestamp:     1483947286.869181     delay_time:           1317  
timestamp:     1483947287.400980     delay_time:           1161  
timestamp:     1483947287.794503     delay_time:           1456  
timestamp:     1483947287.828082     delay_time:           1333  
  
  
  
统计项：
===================================  
服务器时延统计工具，增加时延阈值统计，记录超过阈值的包个数，并把数据包时间戳记录到日志文件，这样可以更加时间戳快速定位到抓包文件，所有参数可配置。  
统计信息:  
timestamp:时间戳  
count:包总数   
max:最大时延   
min:最小时延   
avg:平均时延   
med:按照时延从小到达排序，最中间的时延值，例如100个包，则第50个包的时延为多少   
stddev:每个包的时延平方之和的平均值，和avg  
的平方的差值，该值可以反映时延波动情况    
tc:时延超过40ms的包个数，可以通过-T参数指定这个时间   
95_max:按照时延从小到达排序，第95%的时延值，例如1000个请求，把他们的时延值排序，第%95的时延也就是第950个数组成员的时延值   
95_avg: 按照时延从小到达排序，时延值小的95%时延的平均值 
95_std:按照时延从小到达排序，时延值小的95%的stddev值 
99_max:按照时延从小到达排序，第95%的时延值，例如1000个请求，把他们的时延值排序，第%95的时延也就是第990个数组成员的时延值  
99_avg:按照时延从小到达排序，时延值小的99%时延的平均值
99_std:按照时延从小到达排序，时延值小的99%的stddev值
  
    
使用方法:  
抓包文件离线分析:tcprstat -p 1111 -l 10.2.x.x -t 1 -n 333333 -r xxx.pcap -T 10 -o timetamp.txt  
在线分析:tcprstat -p 1111 -l 10.2.x.x -t 1 -n 333333 -T 20 
  
  
  
  
编译说明:  依赖pthread线程静态库
  
  

编译异常处理:  
===================================
如果出现如下异常，一般是系统netlink库引起的，卸载掉libnl即可  
../libpcap/libpcap-1.1.1//libpcap.a(pcap-linux.o): In function `nl80211_init':  
pcap-linux.c:(.text+0xd1d): undefined reference to `nl_handle_alloc'  
pcap-linux.c:(.text+0xd31): undefined reference to `genl_connect'  
pcap-linux.c:(.text+0xd3d): undefined reference to `genl_ctrl_alloc_cache'  
pcap-linux.c:(.text+0xd59): undefined reference to `genl_ctrl_search_by_name'  
pcap-linux.c:(.text+0xda1): undefined reference to `nl_handle_destroy'  
pcap-linux.c:(.text+0xe0d): undefined reference to `nl_cache_free'  
*/