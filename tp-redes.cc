#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TpRedes");

void setupNodes ();
void simulate ();
void createChannels (NodeContainer emitters, NodeContainer routers, NodeContainer receivers);
void setApplicationLayer (NodeContainer senders, Ipv4InterfaceContainer ir1re0,
                          Ipv4InterfaceContainer ir1re1, NodeContainer receivers);

int
main (int argc, char const *argv[])
{
  std::cout << "setupNodes\n";
  setupNodes ();
  std::cout << "simulate\n";
  simulate ();
  return 0;
}

void
setupNodes ()
{
  NS_LOG_INFO ("Create nodes.");
  NodeContainer senders;
  senders.Create (3);

  NodeContainer routers;
  routers.Create (2);

  NodeContainer receivers;
  receivers.Create (3);

  createChannels (senders, routers, receivers);
}

void
createChannels (NodeContainer senders, NodeContainer routers, NodeContainer receivers)
{
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NodeContainer s0r0;
  s0r0.Add (senders.Get (0));
  s0r0.Add (routers.Get (0));
  NetDeviceContainer devs0r0 = pointToPoint.Install (s0r0);

  NodeContainer s1r0;
  s1r0.Add (senders.Get (1));
  s1r0.Add (routers.Get (0));
  NetDeviceContainer devs1r0 = pointToPoint.Install (s1r0);

  NodeContainer s2r0;
  s2r0.Add (senders.Get (2));
  s2r0.Add (routers.Get (0));
  NetDeviceContainer deve2r0 = pointToPoint.Install (s2r0);

  NetDeviceContainer devRouters = pointToPoint.Install (routers);

  NodeContainer r1re0;
  r1re0.Add (routers.Get (1));
  r1re0.Add (receivers.Get (0));
  NetDeviceContainer devr1re0 = pointToPoint.Install (r1re0);

  NodeContainer r1re1;
  r1re1.Add (routers.Get (1));
  r1re1.Add (receivers.Get (0));
  NetDeviceContainer devr1re1 = pointToPoint.Install (r1re1);

  NodeContainer r1re2;
  r1re2.Add (routers.Get (2));
  r1re2.Add (receivers.Get (0));
  NetDeviceContainer devr1re2 = pointToPoint.Install (r1re2);

  InternetStackHelper internet;
  internet.InstallAll ();

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");

  ipv4.Assign (devs0r0);
  ipv4.Assign (devs1r0);
  ipv4.Assign (deve2r0);
  ipv4.Assign (devRouters);
  Ipv4InterfaceContainer ir1re0 = ipv4.Assign (devr1re0);
  Ipv4InterfaceContainer ir1re1 = ipv4.Assign (devr1re1);
  Ipv4InterfaceContainer ir1re2 = ipv4.Assign (devr1re2);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  setApplicationLayer (senders, ir1re0, ir1re1, receivers);

  //configure tracing
  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tp-redes.tr"));
  pointToPoint.EnablePcapAll ("tp-redes");
}

void
setApplicationLayer (NodeContainer senders, Ipv4InterfaceContainer ir1re0,
                     Ipv4InterfaceContainer ir1re1, NodeContainer receivers)
{
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
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
