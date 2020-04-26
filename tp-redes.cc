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
  std::cout << "Configurando nodos";
  NS_LOG_INFO ("Create nodes.");
  NodeContainer emitters;
  emitters.Create (3);

  NodeContainer routers;
  routers.Create (2);

  NodeContainer receivers;
  receivers.Create (3);

  createChannels (emitters, routers, receivers);
}

void
createChannels (NodeContainer emitters, NodeContainer routers, NodeContainer receivers)
{
  PointToPointHelper pointToPoint;

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NodeContainer e0r0;
  e0r0.Add (emitters.Get (0));
  e0r0.Add (routers.Get (0));
  NetDeviceContainer deve0r0 = pointToPoint.Install (e0r0);

  NodeContainer e1r0;
  e1r0.Add (emitters.Get (1));
  e1r0.Add (routers.Get (0));
  NetDeviceContainer deve1r0 = pointToPoint.Install (e1r0);

  NodeContainer e2r0;
  e2r0.Add (emitters.Get (2));
  e2r0.Add (routers.Get (0));
  NetDeviceContainer deve2r0 = pointToPoint.Install (e2r0);

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

  ipv4.Assign (deve0r0);
  ipv4.Assign (deve1r0);
  ipv4.Assign (deve2r0);
  ipv4.Assign (devRouters);
  ipv4.Assign (devr1re0);
  ipv4.Assign (devr1re1);
  ipv4.Assign (devr1re2);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
}

void
simulate ()
{
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
