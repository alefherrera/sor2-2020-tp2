#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TpRedes");

void setupNodes ();
void simulate (NodeContainer container);
void setApplicationLayer (NodeContainer senders, Ipv4Address receiver0, Ipv4Address receiver1,
                          Ipv4Address receiver2, NodeContainer receivers);
ApplicationContainer setUpApplication (OnOffHelper application, Ptr<Node> source,
                                       Ipv4Address destination, uint16_t port);
OnOffHelper createOnOffApplication (std::string socketFactory);

int
main (int argc, char const *argv[])
{
  setupNodes ();
  return 0;
}

void
setupNodes ()
{
  NS_LOG_UNCOND ("Creating dumbbell");

  PointToPointHelper p2pBottleNeck;
  p2pBottleNeck.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2pBottleNeck.SetChannelAttribute ("Delay", StringValue ("1ms"));

  PointToPointHelper p2pLeft;
  p2pLeft.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2pLeft.SetChannelAttribute ("Delay", StringValue ("1ms"));

  PointToPointHelper p2pRight;
  p2pRight.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2pRight.SetChannelAttribute ("Delay", StringValue ("1ms"));

  PointToPointDumbbellHelper dumbbell (3, p2pLeft, 3, p2pRight, p2pBottleNeck);

  NS_LOG_UNCOND ("SetUp names and positions");
  Names::Add ("sender 0", dumbbell.GetLeft (0));
  AnimationInterface::SetConstantPosition (dumbbell.GetLeft (0), 1, 1);
  Names::Add ("sender 1", dumbbell.GetLeft (1));
  AnimationInterface::SetConstantPosition (dumbbell.GetLeft (1), 1, 3);
  Names::Add ("sender 2", dumbbell.GetLeft (2));
  AnimationInterface::SetConstantPosition (dumbbell.GetLeft (2), 1, 5);

  Names::Add ("router 0", dumbbell.GetLeft ());
  AnimationInterface::SetConstantPosition (dumbbell.GetLeft (), 3, 3);
  Names::Add ("router 1", dumbbell.GetRight ());
  AnimationInterface::SetConstantPosition (dumbbell.GetRight (), 5, 3);

  Names::Add ("receiver 1", dumbbell.GetRight (0));
  AnimationInterface::SetConstantPosition (dumbbell.GetRight (0), 7, 1);
  Names::Add ("receiver 2", dumbbell.GetRight (1));
  AnimationInterface::SetConstantPosition (dumbbell.GetRight (1), 7, 3);
  Names::Add ("receiver 3", dumbbell.GetRight (2));
  AnimationInterface::SetConstantPosition (dumbbell.GetRight (2), 7, 5);

  NS_LOG_UNCOND ("SetUp Stack");

  InternetStackHelper stack;
  NodeContainer senders;
  NodeContainer receivers;
  for (uint32_t i = 0; i < dumbbell.LeftCount (); ++i)
    {
      senders.Add (dumbbell.GetLeft (i));
      stack.Install (dumbbell.GetLeft (i));
    }
  for (uint32_t i = 0; i < dumbbell.RightCount (); ++i)
    {
      receivers.Add (dumbbell.GetRight (i));
      stack.Install (dumbbell.GetRight (i));
    }

  stack.Install (dumbbell.GetLeft ());
  stack.Install (dumbbell.GetRight ());

  NS_LOG_UNCOND ("Assign IPs");

  dumbbell.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                                Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"));

  NS_LOG_UNCOND ("Populating routing tables");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Ipv4Address sender0 = dumbbell.GetLeftIpv4Address (0);
  std::cout << "Sender 0: " << sender0 << "\n";

  Ipv4Address sender1 = dumbbell.GetLeftIpv4Address (1);
  std::cout << "Sender 1: " << sender1 << "\n";

  Ipv4Address sender2 = dumbbell.GetLeftIpv4Address (2);
  std::cout << "Sender 2: " << sender2 << "\n";

  Ipv4Address receiver0 = dumbbell.GetRightIpv4Address (0);
  std::cout << "Receiver 0: " << receiver0 << "\n";

  Ipv4Address receiver1 = dumbbell.GetRightIpv4Address (1);
  std::cout << "Receiver 1: " << receiver1 << "\n";

  Ipv4Address receiver2 = dumbbell.GetRightIpv4Address (2);
  std::cout << "Receiver 2: " << receiver2 << "\n";

  setApplicationLayer (senders, receiver0, receiver1, receiver2, receivers);

  NodeContainer container;
  container.Add (senders);
  container.Add (dumbbell.GetLeft ());
  container.Add (dumbbell.GetRight ());
  container.Add (receivers);
  simulate (container);

  /* AsciiTraceHelper ascii;

  p2pLeft.EnableAsciiAll (ascii.CreateFileStream ("left.tr"));
  p2pLeft.EnablePcapAll ("left");

  p2pRight.EnableAsciiAll (ascii.CreateFileStream ("right.tr"));
  p2pRight.EnablePcapAll ("right");

  p2pBottleNeck.EnableAsciiAll (ascii.CreateFileStream ("bottle-neck.tr"));
  p2pBottleNeck.EnablePcapAll ("bottle-neck"); */
}

OnOffHelper
createOnOffApplication (std::string socketFactory)
{
  OnOffHelper application (socketFactory, Address ());
  application.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  application.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  application.SetAttribute ("DataRate", DataRateValue (DataRate (5 * 8 * 1024 * 1024)));
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
simulate (NodeContainer container)
{
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.Install (container);
  NS_LOG_UNCOND ("Run Simulation");
  AnimationInterface anim ("animation.xml");
  anim.EnablePacketMetadata (true);
  Simulator::Stop (Seconds (10));
  Simulator::Run ();

  NS_LOG_UNCOND ("Start Monitor");
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin ();
       i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress
                << ")\n";
      std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
      std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000 << " Mbps\n";
      std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
      std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000 << " Mbps\n";
      std::cout << "  Lost Packets: " << i->second.lostPackets << "\n";
    }

  monitor->SerializeToXmlFile ("monitor.xml", true, true);

  Simulator::Destroy ();
  NS_LOG_UNCOND ("Done");
}
