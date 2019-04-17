# Datacenter-Network-Simulation-using-ns3

This is the project for the USTC course ：Advanced Computer Networking  
project 1：`Datacenter Network Simulation using ns3`

## Objective
  * Simulate a datacenter network using ns3.
  * Experiment two traffic patterns.
  * Examine the network.
  * Improve the network.

## topology
<div align=center>
    <img width="650" height="206" src="https://github.com/fox6666/VGA-FPGA/blob/master/images/VGA.png" "原理图"/>
</div>

## Description
  * 8 servers: n1\~8; 4 ToR switches: t1~4; 2 aggregation switches: a1~2; 1 core switch: c1
  * The network is partitioned into two clusters
  * The links connecting to c1 are PPP, or the other networks are Ethernets, the networks’ capacities are shown on the topology graph.
  * All the end-end delays on the networks are 500ns.
  * IP address assignment is shown on the topology.
  * All the switches behaves like OSPF routers.
  

