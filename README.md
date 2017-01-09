
/*


install:
yum install perf
yum -y install bison yacc
yum -y install flex
yum install patch
yum install glibc-static

install autoreconf




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
95_max:按照时延从小到达排序，第95%的时延值，例如1000
个请求，把他们的时延值排序，第%95的时延也就是第950个数组成员的时延值 

95_avg: 按照时延从小到达排序，时延值小的95%时延的平均值 
95_std:按照时延从小到达排序，时延值小的95%的stddev值 
99_max:按照时延从小到达排序，第95%的时延值，例如1000
个请求，把他们的时延值排序，第%95的时延也就是第990个数组成员的时延值 

99_avg:按照时延从小到达排序，时延值小的99%时延的平均值
99_std:按照时延从小到达排序，时延值小的99%的stddev值



命令行参数:
-p:端口
-l:ip
-o:打印时延超过-T参数指定的包的数量
-T:时延统计阈值，超过该时间的会通过tc打印超过该时延阈值的包个数
-o:如果有包的时延超过阈值，则把该包的时间戳记录到该文件中，这样可以帮助分析报文
-t:统计打印时间间隔
-n:最多打印这么多次
-r:离线统计功能的抓包文件



使用方法:

抓包文件离线分析:tcprstat -p 1111 -l 10.2.x.x -t 1 -n 333333 -r xxx.pcap -T 10 -o 
timetamp.txt

在线分析:tcprstat -p 1111 -l 10.2.x.x -t 1 -n 333333 





编译说明:  依赖pthread线程静态库
编译tcprstat

在RHEL6.1(Red Hat Enterprise Linux Server)上静态编译并不容易。tcprstat
编译也有这个问题。


源码下载：tcprstat@Launchpad 命令：bzr branch lp:tcprstat

编译命令：./bootstrap && ./configure && make

如果顺利的话，就结束了。不过在我的发行版会报如下错误：

gcc -Wall -Werror -g -pthread -I../libpcap/libpcap-1.1.1/ -g -O2 -static -L../
libpcap/libpcap-1.1.1/ \

-o tcprstat-static tcprstat_static-tcprstat.o tcprstat_static-functions.o 
tcprstat_static-capture.o \

tcprstat_static-process-packet.o tcprstat_static-local-addresses.o 
tcprstat_static-stats.o \

tcprstat_static-output.o tcprstat_static-stats-hash.o -lpthread -lpcap -L/usr/
lib64/

ld: cannot find -lpthread
collect2: ld returned 1 exit status
经张新铭同学指点，发现在尝试静态编译时(-static)，找不到pthread的链接库。
pthread在Linux的glibc库中实现(NPTL)，所以需要静态编译链接，则需要对应的glibc
静态链接库。


查询这台机器对应的glibc版本为glibc-2.12-1.25.el6.x86_64(rpm -qa|grep glibc)
，那么也需要对应版本的静态链接库，在网上搜索对应的rpm包，并安装：


glibc-static-2.12-1.25.el6.x86_64.rpm
rpm -ivh glibc-static-2.12-1.25.el6.x86_64.rpm
问题解决。编译好的文件再src目录下：tcprstat tcprstat-static

在RHEL上静态链接库缺失导致编译失败的问题很常见，故记录之。
*/