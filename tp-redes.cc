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
void setApplicationLayer (NodeContainer senders, Ipv4InterfaceContainer ir1re0,
                          Ipv4InterfaceContainer ir1re1, NodeContainer receivers);
NetDeviceContainer createDevice (PointToPointHelper pointToPoint, NodeContainer node1,
                                 NodeContainer node2);

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

  NS_LOG_UNCOND ("Creating routers");
  NodeContainer routers;
  routers.Create (2);

  NS_LOG_UNCOND ("Creating receivers");
  NodeContainer receivers;
  receivers.Create (3);

  createChannels (senders, routers, receivers);
}

void
createChannels (NodeContainer senders, NodeContainer routers, NodeContainer receivers)
{
  NS_LOG_UNCOND ("Creating channels");
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

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

  NS_LOG_UNCOND ("Installing internet");
  InternetStackHelper internet;
  internet.InstallAll ();

  NS_LOG_UNCOND ("Assigning Ips");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");

  ipv4.Assign (devs0r0);
  ipv4.Assign (devs1r0);
  ipv4.Assign (deve2r0);
  ipv4.Assign (devRouters);

  Ipv4InterfaceContainer ir1re0 = ipv4.Assign (devr1re0);
  Ipv4InterfaceContainer ir1re1 = ipv4.Assign (devr1re1);
  Ipv4InterfaceContainer ir1re2 = ipv4.Assign (devr1re2);

  NS_LOG_UNCOND ("Populating routing tables");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  setApplicationLayer (senders, ir1re0, ir1re1, receivers);

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

void
setApplicationLayer (NodeContainer senders, Ipv4InterfaceContainer ir1re0,
                     Ipv4InterfaceContainer ir1re1, NodeContainer receivers)
{
  NS_LOG_UNCOND ("Setting up Application layer");

  // Create the OnOff applications to send TCP to the server
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  uint16_t port = 50000;
  //normally wouldn't need a loop here but the server IP address is different
  //on each p2p subnet
  ApplicationContainer clientApps;

  AddressValue remoteAddress1 (InetSocketAddress (ir1re0.GetAddress (1), port));
  clientHelper.SetAttribute ("Remote", remoteAddress1);
  clientApps.Add (clientHelper.Install (senders.Get (0)));

  AddressValue remoteAddress2 (InetSocketAddress (ir1re1.GetAddress (1), port));
  clientHelper.SetAttribute ("Remote", remoteAddress2);
  clientApps.Add (clientHelper.Install (senders.Get (1)));

  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (10.0));

  uint16_t servPort = 50000;
  // Create a packet sink to receive these packets on n2...
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), servPort));

  ApplicationContainer receiverApps;
  receiverApps.Add (sink.Install (receivers.Get (0)));
  receiverApps.Add (sink.Install (receivers.Get (1)));
  receiverApps.Add (sink.Install (receivers.Get (2)));
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
