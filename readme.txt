实验报告：
      Datacenter Network Simulation using ns3.docx  是本次实验报告word版，有标题可能会看着方便些，若格式不对，请看PDF版；
      Datacenter Network Simulation using ns3.pdf  是本次实验报告PDF版，里面有代码解释、仿真结果（跟踪，图形，统计）、瓶颈分析、瓶颈改善的结果与分析；

源码说明：
      dcn1.cc和dcn2.cc分别是模式1和模式2的仿真源码；
      dcn1_add_1core.cc、dcn1_add_3core.cc和dcn1_add_3core_1.5Mbps.cc分别是模式1通过增加core switch进行瓶颈改进：
	增加1个core switch、增加3个core switch和增加3core switch，中间层带宽加为1.5Mbps；
      dcn1_2Mbps.cc、dcn1_3Mbps.cc和dcn1_top3M_mid2M.cc分别是模式1通过增加顶层带宽进行瓶颈改进：
	顶层带宽增加为2Mbps、3Mbps和顶层3Mbps，中间层2Mbps；
      dcn2_2Mbps.cc和dcn2_3Mbps.cc分别是模式2通过增加带宽改进瓶颈：
	底层带宽增加为2Mbps和3Mbps；

文件夹说明：
      pattern1：里面放置是模式1下仿真抓取的.pcap包，以及瓶颈改进后抓取的.pcap包，可以根据包名知道哪一文件得到的仿真包，在实验报告中也有说明；
      pattern2：里面放置是模式2下仿真抓取的.pcap包，以及瓶颈改进后抓取的.pcap包，可以根据包名知道哪一文件得到的仿真包，在实验报告中也有说明；
      
