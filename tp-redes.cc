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

  NodeContainer e0r0;
  e0r0.Add (emitters.Get (0));
  e0r0.Add (routers.Get (0));
  pointToPoint.Install (e0r0);

  NodeContainer e1r0;
  e1r0.Add (emitters.Get (1));
  e1r0.Add (routers.Get (0));
  pointToPoint.Install (e1r0);

  NodeContainer e2r0;
  e2r0.Add (emitters.Get (2));
  e2r0.Add (routers.Get (0));
  pointToPoint.Install (e2r0);

  pointToPoint.Install (routers);

  NodeContainer r1re0;
  r1re0.Add (routers.Get (1));
  r1re0.Add (receivers.Get (0));
  pointToPoint.Install (r1re0);

  NodeContainer r1re1;
  r1re1.Add (routers.Get (1));
  r1re1.Add (receivers.Get (0));
  pointToPoint.Install (r1re1);

  NodeContainer r1re2;
  r1re2.Add (routers.Get (2));
  r1re2.Add (receivers.Get (0));
  pointToPoint.Install (r1re2);

  InternetStackHelper internet;
  internet.InstallAll ();
}

void
simulate ()
{
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
