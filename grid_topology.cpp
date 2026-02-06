#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
    // Create 25 nodes (5x5 grid mesh)
    NodeContainer nodes;
    nodes.Create (25);

    // Point-to-point helper
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    // Install Internet
    InternetStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    int subnet = 1;

    int grid = 5;

    // Connect neighbors
    for (int i = 0; i < grid; i++)
    {
        for (int j = 0; j < grid; j++)
        {
            int index = i * grid + j;

            // Right
            if (j < grid - 1)
            {
                NodeContainer pair (nodes.Get (index),
                                    nodes.Get (index + 1));

                NetDeviceContainer dev = p2p.Install (pair);

                std::ostringstream s;
                s << "10.3." << subnet++ << ".0";

                address.SetBase (s.str ().c_str (), "255.255.255.0");
                address.Assign (dev);
            }

            // Down
            if (i < grid - 1)
            {
                NodeContainer pair (nodes.Get (index),
                                    nodes.Get (index + grid));

                NetDeviceContainer dev = p2p.Install (pair);

                std::ostringstream s;
                s << "10.3." << subnet++ << ".0";

                address.SetBase (s.str ().c_str (), "255.255.255.0");
                address.Assign (dev);
            }
        }
    }

    // Mobility
    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
        "MinX", DoubleValue (50.0),
        "MinY", DoubleValue (50.0),
        "DeltaX", DoubleValue (100.0),
        "DeltaY", DoubleValue (100.0),
        "GridWidth", UintegerValue (5),
        "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);

    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
