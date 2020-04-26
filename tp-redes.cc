#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TpRedes");

void setupNodes ();
void simulate ();

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
  NodeContainer n;
  n.Create (4);
}

void
simulate ()
{
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
