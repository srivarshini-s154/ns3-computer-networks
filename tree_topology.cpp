#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
    // Root node
    NodeContainer root;
    root.Create (1);

    // Level 1 (children)
    NodeContainer level1;
    level1.Create (4);

    // Level 2 (leaves)
    NodeContainer level2;
    level2.Create (16);

    // Point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // Internet stack
    InternetStackHelper stack;
    stack.Install (root);
    stack.Install (level1);
    stack.Install (level2);

    Ipv4AddressHelper address;
    int subnet = 1;

    // Connect root to level1
    for (uint32_t i = 0; i < level1.GetN (); i++)
    {
        NodeContainer pair (root.Get (0), level1.Get (i));

        NetDeviceContainer dev = p2p.Install (pair);

        std::ostringstream s;
        s << "10.5." << subnet++ << ".0";

        address.SetBase (s.str ().c_str (), "255.255.255.0");
        address.Assign (dev);
    }

    // Connect level1 to level2 (4 each)
    int index = 0;

    for (uint32_t i = 0; i < level1.GetN (); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            NodeContainer pair (level1.Get (i), level2.Get (index++));

            NetDeviceContainer dev = p2p.Install (pair);

            std::ostringstream s;
            s << "10.5." << subnet++ << ".0";

            address.SetBase (s.str ().c_str (), "255.255.255.0");
            address.Assign (dev);
        }
    }

    // Mobility
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    // Root position
    mobility.Install (root);
    root.Get (0)->GetObject<MobilityModel> ()
        ->SetPosition (Vector (300.0, 50.0, 0.0));

    // Level1 positions
    mobility.Install (level1);

    for (uint32_t i = 0; i < level1.GetN (); i++)
    {
        double x = 150.0 + i * 150;
        double y = 150.0;

        level1.Get (i)->GetObject<MobilityModel> ()
            ->SetPosition (Vector (x, y, 0.0));
    }

    // Level2 positions
    mobility.Install (level2);

    int k = 0;

    for (uint32_t i = 0; i < level1.GetN (); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            double x = 100.0 + i * 150 + j * 30;
            double y = 250.0;

            level2.Get (k++)->GetObject<MobilityModel> ()
                ->SetPosition (Vector (x, y, 0.0));
        }
    }

    // NetAnim
    AnimationInterface anim ("topo5_tree.xml");

    // Run
    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
