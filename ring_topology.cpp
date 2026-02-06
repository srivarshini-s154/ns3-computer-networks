#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

#include <cmath>

using namespace ns3;

int main (int argc, char *argv[])
{
    // Create 20 nodes
    NodeContainer nodes;
    nodes.Create (20);

    // Point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // Install Internet
    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;

    // Connect nodes in ring
    for (uint32_t i = 0; i < nodes.GetN (); i++)
    {
        NodeContainer pair;

        if (i == nodes.GetN () - 1)
        {
            // Last connects to first
            pair = NodeContainer (nodes.Get (i), nodes.Get (0));
        }
        else
        {
            pair = NodeContainer (nodes.Get (i), nodes.Get (i+1));
        }

        NetDeviceContainer devices = p2p.Install (pair);

        std::ostringstream subnet;
        subnet << "10.2." << i+1 << ".0";

        address.SetBase (subnet.str ().c_str (), "255.255.255.0");
        address.Assign (devices);
    }

    // Mobility: arrange in circle
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);

    double radius = 250.0;
    double angleStep = 2 * M_PI / nodes.GetN ();

    for (uint32_t i = 0; i < nodes.GetN (); i++)
    {
        double angle = i * angleStep;

        double x = 300.0 + radius * cos (angle);
        double y = 300.0 + radius * sin (angle);

        Ptr<MobilityModel> mob = nodes.Get (i)->GetObject<MobilityModel> ();
        mob->SetPosition (Vector (x, y, 0.0));
    }

    // NetAnim
    AnimationInterface anim ("topo3_ring.xml");

    // Run
    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
