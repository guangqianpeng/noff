# noff

TCP session: **src_ip** | **src_port** | **dst_ip**| **dst_port** | 

​                       **up_flow_bytes** | **#up_packet** | **#up_syn** | **#up_fin** | **#up_psh** | **#up_rst** | 

​                      \#**up_small_packet** | **#up_rxmit_packe**t | **#up_rtt**

​                       **down_flow_bytes** | **#down_packet** | **#down_syn** | **#down_fin** | **#down_psh** | **#dow_rst** | 

​                      \#**down_small_packet** | **#down_rxmit_packe**t | **#down_rtt**

​                      **interval_in_seconds** | **nomally_closed_or_not** | **packet_size_1** | **packet_size_2** | ... 

依赖库：

1. [muduo](https://github.com/chenshuo/muduo)最新的master分支，将编译生成的build目录放到项目目录下
2. [pfring](https://github.com/ntop/PF_RING)，直接编译安装
3. pcap，sudo apt install libpcap-dev
4. tcmalloc，sudo apt install google-perftools

使用：

./noff interface #channel



