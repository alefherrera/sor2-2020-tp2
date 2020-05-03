#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TpRedes");

void setupNodes ();
void simulate ();
void createChannels (NodeContainer emitters, NodeContainer routers, NodeContainer receivers);
void setApplicationLayer (NodeContainer senders, Ipv4Address receiver0, Ipv4Address receiver1,
                          Ipv4Address receiver2, NodeContainer receivers);
NetDeviceContainer createDevice (PointToPointHelper pointToPoint, NodeContainer node1,
                                 NodeContainer node2);
ApplicationContainer setUpApplication (OnOffHelper application, Ptr<Node> source,
                                       Ipv4Address destination, uint16_t port);
OnOffHelper createOnOffApplication (std::string socketFactory);

int
main (int argc, char const *argv[])
{
  setupNodes ();
  simulate ();
  return 0;
}

void
setupNodes ()
{
  NS_LOG_UNCOND ("Creating senders");
  NodeContainer senders;
  senders.Create (3);

  Names::Add ("sender 1", senders.Get (0));
  AnimationInterface::SetConstantPosition (senders.Get (0), 1, 1);
  Names::Add ("sender 2", senders.Get (1));
  AnimationInterface::SetConstantPosition (senders.Get (1), 1, 3);
  Names::Add ("sender 3", senders.Get (2));
  AnimationInterface::SetConstantPosition (senders.Get (2), 1, 5);

  NS_LOG_UNCOND ("Creating routers");
  NodeContainer routers;
  routers.Create (2);

  Names::Add ("router 1", routers.Get (0));
  AnimationInterface::SetConstantPosition (routers.Get (0), 3, 3);
  Names::Add ("router 2", routers.Get (1));
  AnimationInterface::SetConstantPosition (routers.Get (1), 5, 3);

  NS_LOG_UNCOND ("Creating receivers");
  NodeContainer receivers;
  receivers.Create (3);

  Names::Add ("receiver 1", receivers.Get (0));
  AnimationInterface::SetConstantPosition (receivers.Get (0), 7, 1);
  Names::Add ("receiver 2", receivers.Get (1));
  AnimationInterface::SetConstantPosition (receivers.Get (1), 7, 3);
  Names::Add ("receiver 3", receivers.Get (2));
  AnimationInterface::SetConstantPosition (receivers.Get (2), 7, 5);

  createChannels (senders, routers, receivers);
}

void
createChannels (NodeContainer senders, NodeContainer routers, NodeContainer receivers)
{
  NS_LOG_UNCOND ("Creating channels");
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NS_LOG_UNCOND ("Installing internet");
  InternetStackHelper internet;
  internet.Install (senders);
  internet.Install (routers);
  internet.Install (receivers);

  NS_LOG_UNCOND ("Installing point to point in s0r0");
  NetDeviceContainer devs0r0 = createDevice (pointToPoint, senders.Get (0), routers.Get (0));

  NS_LOG_UNCOND ("Installing point to point in s1r0");
  NetDeviceContainer devs1r0 = createDevice (pointToPoint, senders.Get (1), routers.Get (0));

  NS_LOG_UNCOND ("Installing point to point in s2r0");
  NetDeviceContainer deve2r0 = createDevice (pointToPoint, senders.Get (2), routers.Get (0));

  NS_LOG_UNCOND ("Installing point to point between routers");
  NetDeviceContainer devRouters = pointToPoint.Install (routers);

  NS_LOG_UNCOND ("Installing point to point in r1re0");
  NetDeviceContainer devr1re0 = createDevice (pointToPoint, routers.Get (1), receivers.Get (0));

  NS_LOG_UNCOND ("Installing point to point in r1re1");
  NetDeviceContainer devr1re1 = createDevice (pointToPoint, routers.Get (1), receivers.Get (1));

  NS_LOG_UNCOND ("Installing point to point in r1re2");
  NetDeviceContainer devr1re2 = createDevice (pointToPoint, routers.Get (1), receivers.Get (2));

  NS_LOG_UNCOND ("Assigning Ips");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (devs0r0);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4.Assign (devs1r0);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign (deve2r0);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4.Assign (devRouters);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer ir1re0 = ipv4.Assign (devr1re0);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer ir1re1 = ipv4.Assign (devr1re1);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer ir1re2 = ipv4.Assign (devr1re2);

  NS_LOG_UNCOND ("Populating routing tables");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_UNCOND ("Receiver 0");
  Ipv4Address receiver0 = ir1re0.GetAddress (1);
  NS_LOG_UNCOND (receiver0);

  NS_LOG_UNCOND ("Receiver 1");
  Ipv4Address receiver1 = ir1re1.GetAddress (1);
  NS_LOG_UNCOND (receiver1);

  NS_LOG_UNCOND ("Receiver 2");
  Ipv4Address receiver2 = ir1re2.GetAddress (1);
  NS_LOG_UNCOND (receiver2);

  setApplicationLayer (senders, receiver0, receiver1, receiver2, receivers);

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tp-redes.tr"));
  pointToPoint.EnablePcapAll ("tp-redes");
}

NetDeviceContainer
createDevice (PointToPointHelper pointToPoint, NodeContainer node1, NodeContainer node2)
{
  NodeContainer result;
  result.Add (node1);
  result.Add (node2);
  return pointToPoint.Install (result);
}

OnOffHelper
createOnOffApplication (std::string socketFactory)
{
  OnOffHelper application (socketFactory, Address ());
  application.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  application.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  return application;
}

ApplicationContainer
setUpApplication (OnOffHelper application, Ptr<Node> source, Ipv4Address destination, uint16_t port)
{
  AddressValue address (InetSocketAddress (destination, port));
  application.SetAttribute ("Remote", address);
  return application.Install (source);
}

void
setApplicationLayer (NodeContainer senders, Ipv4Address receiver0, Ipv4Address receiver1,
                     Ipv4Address receiver2, NodeContainer receivers)
{
  NS_LOG_UNCOND ("Setting up Application layer");

  // Create the OnOff applications to send TCP to the server
  OnOffHelper tcpOnOffApplication = createOnOffApplication ("ns3::TcpSocketFactory");
  OnOffHelper udpOnOffApplication = createOnOffApplication ("ns3::UdpSocketFactory");

  uint16_t tcpPort = 50000;
  uint16_t udpPort = 50001;

  ApplicationContainer senderApps;

  // set up sender0 with onOff over TCP to send to receiver0
  senderApps.Add (setUpApplication (tcpOnOffApplication, senders.Get (0), receiver0, tcpPort));
  // set up sender1 with onOff over TCP to send to receiver1
  senderApps.Add (setUpApplication (tcpOnOffApplication, senders.Get (1), receiver1, tcpPort));
  // set up sender2 with onOff over UDP to send to receiver2
  senderApps.Add (setUpApplication (udpOnOffApplication, senders.Get (2), receiver2, udpPort));

  senderApps.Start (Seconds (1.0));
  senderApps.Stop (Seconds (10.0));

  PacketSinkHelper tcpSink ("ns3::TcpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), tcpPort));

  PacketSinkHelper udpSink ("ns3::UdpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), udpPort));

  ApplicationContainer receiverApps;
  receiverApps.Add (tcpSink.Install (receivers.Get (0)));
  receiverApps.Add (tcpSink.Install (receivers.Get (1)));
  receiverApps.Add (udpSink.Install (receivers.Get (2)));
  receiverApps.Start (Seconds (0.0));
  receiverApps.Stop (Seconds (10.0));
}

void
simulate ()
{
  NS_LOG_UNCOND ("Run Simulation");
  AnimationInterface anim ("animation.xml");
  anim.EnablePacketMetadata (true);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_UNCOND ("Done");
}
