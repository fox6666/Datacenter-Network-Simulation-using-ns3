#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

//8 servers: n1~8; 4 ToR switches: t1~4;
//2 aggregation switches: a1~2; 1 core switch: c1
//The network is partitioned/ into two clusters
//
//                                          c1
//                                       -     -  
//                                     -          -
//                 3.0Mbps           -              -       3.0Mbps
//              192.168.1.0/24     -                  -  192.168.2.0/24
//                               -                      -
//                             -                           -
//                           -                                -
//     Cluster1            -                       clusters2     -
//                      a1                                         a2
//      1.0Mbps         |                          1.0Mbps          |
//   10.1.1.0/24  ==============                 10.2.1.0/24  ============== 
//                  |         |                                 |         |
//                  t1        t2                                t3        t4
//     1.0Mbps      |         |       1.0Mbps      1.0Mbps      |         |       1.0Mbps    
//   10.0.1.0/24 ========  ======== 10.0.2.0/24  10.0.3.0/24 ========  ======== 10.0.4.0/24  
//                |    |    |    |                            |    |    |    |
//                n1   n2   n3   n4                           n5   n6   n7   n8


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DataCenter_Network");

int main(int argc, char *argv[])
{

    bool verbose = true;

    CommandLine cmd;
    //添加自己的变量 （属性名称，属性说明，变量）
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse (argc,argv);
    
    if (verbose){
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    /************网络拓扑部分*************/
    //创建2个aggregation:a1,a2   core-->a1  core-->a2   ptpNodes
    //a1 = aggregation1.Get(1)   a2 = aggregation2.Get(1)  core = aggregation1.Get(0)
    NodeContainer aggregation1;
    aggregation1.Create(2);   //c1,a1

    NodeContainer aggregation2;
    aggregation2.Add(aggregation1.Get(0));//C1
    aggregation2.Create(1);  //a2
    
    //中间网络部分 ：a1-->t1,t2  a2-->t3,t4
    //创建ToR
    NodeContainer toR1,toR2;
    toR1.Add(aggregation1.Get(1));//a1
    toR1.Create(2);  //t1,t2
    toR2.Add(aggregation2.Get(1));//a2
    toR2.Create(2);  //t3,t4

    //底层网络部分：t1-->n1,n2  t2-->n3,n4  t3-->n5,n6  t4-->n7,n8
    //创建节点Nodes   csmaNodes
    NodeContainer csmaNode1,csmaNode2,csmaNode3,csmaNode4;
    csmaNode1.Add(toR1.Get(1));//t1
    csmaNode1.Create(2);//n1,n2
    csmaNode2.Add(toR1.Get(2));//t2
    csmaNode2.Create(2);//n3,n4
    csmaNode3.Add(toR2.Get(1));//t3
    csmaNode3.Create(2);//n5,n6
    csmaNode4.Add(toR2.Get(2));//t4
    csmaNode4.Create(2);//n7,n8

    //设置传送速率和信道延迟
    PointToPointHelper pointToPoint1,pointToPoint2;
    pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("3.0Mbps"));
    pointToPoint1.SetChannelAttribute ("Delay"  , TimeValue(NanoSeconds(500)) );
    pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("3.0Mbps"));
    pointToPoint2.SetChannelAttribute ("Delay"  , TimeValue(NanoSeconds(500)) );

    //安装P2P网卡设备到P2P网络节点
    NetDeviceContainer p2pDevices1,p2pDevices2;
    p2pDevices1 = pointToPoint1.Install(aggregation1);
    p2pDevices2 = pointToPoint2.Install(aggregation2);

    //创建和连接CSMA设备及信道
    CsmaHelper csma2,csma3;
    csma2.SetChannelAttribute("DataRate", StringValue("1Mbps"));
    csma2.SetChannelAttribute("Delay"   , TimeValue(NanoSeconds(500)) );
    csma3.SetChannelAttribute("DataRate", StringValue("1Mbps"));
    csma3.SetChannelAttribute("Delay"   , TimeValue(NanoSeconds(500)) );

    //中间层csma
    NetDeviceContainer toR1Devices1,toR2Devices2;
    toR1Devices1 = csma2.Install(toR1);
    toR2Devices2 = csma2.Install(toR2);
    //底层 csma
    NetDeviceContainer csmaNode1Devices1,csmaNode1Devices2,csmaNode1Devices3,csmaNode1Devices4;
    csmaNode1Devices1 = csma3.Install(csmaNode1);
    csmaNode1Devices2 = csma3.Install(csmaNode2);
    csmaNode1Devices3 = csma3.Install(csmaNode3);
    csmaNode1Devices4 = csma3.Install(csmaNode4);

    //安装网络协议
    InternetStackHelper stack;
    stack.Install (aggregation1);
    stack.Install (aggregation2.Get(1));
    stack.Install (toR1.Get(1));
    stack.Install (toR1.Get(2));
    stack.Install (toR2.Get(1));
    stack.Install (toR2.Get(2));
    stack.Install (csmaNode1.Get(1));
    stack.Install (csmaNode1.Get(2));
    stack.Install (csmaNode2.Get(1));
    stack.Install (csmaNode2.Get(2));
    stack.Install (csmaNode3.Get(1));
    stack.Install (csmaNode3.Get(2));
    stack.Install (csmaNode4.Get(1));
    stack.Install (csmaNode4.Get(2));

    //两个网段的IP地址类对象
    Ipv4AddressHelper address;
    //安排P2P网段的地址
    address.SetBase("192.168.1.0","255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces1 = address.Assign(p2pDevices1);
    address.SetBase("192.168.2.0","255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);

    //安排CSMA网段地址
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer toRInterfaces1 = address.Assign(toR1Devices1);
    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer toRInterfaces2 = address.Assign(toR2Devices2);
    address.SetBase("10.0.1.0","255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces1 = address.Assign(csmaNode1Devices1);
    address.SetBase("10.0.2.0","255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces2 = address.Assign(csmaNode1Devices2);
    address.SetBase("10.0.3.0","255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces3 = address.Assign(csmaNode1Devices3);
    address.SetBase("10.0.4.0","255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces4 = address.Assign(csmaNode1Devices4);
    /************网络拓扑部分结束************/

    /**************应用程序部分**************/
    //Pattern 1 :  inter-cluster traffic
    //Each server communicates using TCP with another server that comes from different cluster
    //For example, 1-5, 6-2, 3-7, 8-4
    int16_t sinkPort = 8080;
    
    //建立 traffic : 1-5
    PacketSinkHelper packetSinkHelper1("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces3.GetAddress(1),sinkPort));//n5
    ApplicationContainer sinkApp1 = packetSinkHelper1.Install(csmaNode3.Get(1));
    sinkApp1.Start(Seconds(0.0));
    sinkApp1.Stop(Seconds(20.0));

    OnOffHelper client1("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces3.GetAddress(1),sinkPort));
    client1.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
    client1.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    client1.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
    client1.SetAttribute ("PacketSize", UintegerValue (2000));
    ApplicationContainer clientApp1 = client1.Install (csmaNode1.Get (1));
    clientApp1.Start(Seconds (1.0 ));
    clientApp1.Stop (Seconds (21.0));
    //使用csma网段第二个节点进行sniff，True开启混杂模式
    //csma3.EnablePcap ("Pattern1_n1_to_n5",csmaNode1Devices3.Get(1),true);


    //建立 traffic : n6-n2
    PacketSinkHelper packetSinkHelper2("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces1.GetAddress(2),sinkPort));//n2
    ApplicationContainer sinkApp2 = packetSinkHelper2.Install(csmaNode1.Get(2));
    sinkApp2.Start(Seconds(0.0));
    sinkApp2.Stop(Seconds(20.0));

    OnOffHelper client2("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces1.GetAddress(2),sinkPort));
    client2.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
    client2.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    client2.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
    client2.SetAttribute ("PacketSize", UintegerValue (2000));
    ApplicationContainer clientApp2 = client2.Install (csmaNode3.Get (2));
    clientApp2.Start(Seconds (1.0 ));
    clientApp2.Stop (Seconds (21.0));
    //使用csma网段第二个节点进行sniff，True开启混杂模式
   // csma3.EnablePcap ("Pattern1_n6_to_n2",csmaNode1Devices1.Get(2),true);


    //建立 traffic : n3-n7
    PacketSinkHelper packetSinkHelper3("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces4.GetAddress(1),sinkPort));//n7
    ApplicationContainer sinkApp3 = packetSinkHelper3.Install(csmaNode4.Get(1));
    sinkApp3.Start(Seconds(0.0));
    sinkApp3.Stop(Seconds(20.0));

    OnOffHelper client3("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces4.GetAddress(1),sinkPort));
    client3.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
    client3.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    client3.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
    client3.SetAttribute ("PacketSize", UintegerValue (2000));
    ApplicationContainer clientApp3 = client3.Install (csmaNode2.Get (1));
    clientApp3.Start(Seconds (1.0 ));
    clientApp3.Stop (Seconds (21.0));
    //使用csma网段第二个节点进行sniff，True开启混杂模式
    //csma3.EnablePcap ("Pattern1_n3_to_n7",csmaNode1Devices4.Get(1),true);


    //建立 traffic : n8-n4
    PacketSinkHelper packetSinkHelper4("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces2.GetAddress(2),sinkPort));//n4
    ApplicationContainer sinkApp4 = packetSinkHelper4.Install(csmaNode2.Get(2));
    sinkApp4.Start(Seconds(0.0));
    sinkApp4.Stop(Seconds(20.0));

    OnOffHelper client4("ns3::TcpSocketFactory",InetSocketAddress(csmaInterfaces2.GetAddress(2),sinkPort));
    client4.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
    client4.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    client4.SetAttribute ("DataRate", DataRateValue (DataRate ("1.0Mbps")));
    client4.SetAttribute ("PacketSize", UintegerValue (2000));
    ApplicationContainer clientApp4 = client4.Install (csmaNode4.Get (2));
    clientApp4.Start(Seconds (1.0 ));
    clientApp4.Stop (Seconds (21.0));
    //使用csma网段第二个节点进行sniff，True开启混杂模式
    //csma3.EnablePcap ("Pattern1_n8_to_n4",csmaNode1Devices2.Get(2),true);

    
    /****调用全局路由,帮助建立网络路由****/
    //全局路由管理器根据节点产生的链路通告为每个节点建立路由表
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    /***********开启pcap跟踪**************/
    //生成.pcap文件，便于使用Wireshark分析跟踪
    pointToPoint1.EnablePcap ("Pattern1_3Mbps_c1",p2pDevices1.Get(0), true);
    csma2.EnablePcap ("Pattern1_3Mbps_t1",toR1Devices1.Get(1), true);
    csma2.EnablePcap ("Pattern1_3Mbps_t3",toR2Devices2.Get(1), true);
    csma3.EnablePcap ("Pattern1_3Mbps_n1",csmaNode1Devices3.Get(1),true);
    csma3.EnablePcap ("Pattern1_3Mbps_n6",csmaNode1Devices1.Get(2),true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
    
}




