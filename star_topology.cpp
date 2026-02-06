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
    // 1 center + 20 leaves = 21 nodes
    NodeContainer center;
    center.Create (1);

    NodeContainer leaves;
    leaves.Create (20);

    // Point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // Internet stack
    InternetStackHelper stack;
    stack.Install (center);
    stack.Install (leaves);

    Ipv4AddressHelper address;

    // Connect each leaf to center
    for (uint32_t i = 0; i < leaves.GetN (); i++)
    {
        NodeContainer pair (center.Get (0), leaves.Get (i));

        NetDeviceContainer devices = p2p.Install (pair);

        std::ostringstream subnet;
        subnet << "10.1." << i+1 << ".0";

        address.SetBase (subnet.str ().c_str (), "255.255.255.0");
        address.Assign (devices);
    }

    // Mobility for center
    MobilityHelper mobilityCenter;
    mobilityCenter.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobilityCenter.Install (center);

    Ptr<MobilityModel> mob = center.Get (0)->GetObject<MobilityModel> ();
    mob->SetPosition (Vector (300.0, 300.0, 0.0));

    // Mobility for leaves (manual circle)
    MobilityHelper mobilityLeaves;
    mobilityLeaves.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobilityLeaves.Install (leaves);

    double radius = 200.0;
    double angleStep = 2 * M_PI / leaves.GetN ();

    for (uint32_t i = 0; i < leaves.GetN (); i++)
    {
        double angle = i * angleStep;

        double x = 300.0 + radius * cos (angle);
        double y = 300.0 + radius * sin (angle);

        Ptr<MobilityModel> m = leaves.Get (i)->GetObject<MobilityModel> ();
        m->SetPosition (Vector (x, y, 0.0));
    }

    // NetAnim
    AnimationInterface anim ("topo2_star.xml");

    // Run
    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
