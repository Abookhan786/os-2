# ==============================================================================
# Question 6: 6-Node Mesh Topology in NS2 with Dynamic Route Discovery
# ==============================================================================
# Description:
#   - Implements a Full 6-Node Mesh Topology (n0 through n5).
#   - Every node connects to every other node (15 duplex links total).
#   - Uses Distance Vector (DV) dynamic routing ($ns rtproto DV).
#   - Generates UDP/CBR traffic between selected pair: n0 (source) and n3 (destination).
#   - Demonstrates multi-path route discovery and dynamic rerouting:
#       * 0.5s - 2.0s : Direct route (n0 -> n3)
#       * 2.0s - 3.5s : Direct link n0-n3 goes DOWN; DV protocol reroutes
#                       via alternate path (e.g. n0 -> n1 -> n3 or n0 -> n2 -> n3)
#       * 3.5s - 4.5s : Direct link n0-n3 restored UP; traffic reverts to direct route
#   - Visualizes simulation and active routes in NAM.
#
# How to Run:
#   ns q6_six_node_mesh_topology.tcl
# ==============================================================================

# 1. Initialize Simulator
set ns [new Simulator]

# Enable Dynamic Distance Vector Routing Protocol
$ns rtproto DV

# Color coding for traffic visualization
$ns color 1 Red

# 2. Open Trace Files
set tf [open q6_mesh_topology.tr w]
$ns trace-all $tf

set nf [open q6_mesh_topology.nam w]
$ns namtrace-all $nf

# 3. Finish Procedure
proc finish {} {
    global ns tf nf
    $ns flush-trace
    close $tf
    close $nf
    puts "=========================================================="
    puts "   6-Node Mesh Topology Simulation Finished               "
    puts "=========================================================="
    puts "Topology       : 6-Node Full Mesh (15 Duplex Links)"
    puts "Traffic Pair   : Source = Node 0 (n0), Destination = Node 3 (n3)"
    puts "Primary Route  : Direct link n0 -> n3"
    puts "Alternate Paths: n0 -> n1 -> n3"
    puts "                 n0 -> n2 -> n3"
    puts "                 n0 -> n4 -> n3"
    puts "                 n0 -> n5 -> n3"
    puts "Trace File     : q6_mesh_topology.tr"
    puts "NAM File       : q6_mesh_topology.nam"
    puts "Launching NAM..."
    exec nam q6_mesh_topology.nam &
    exit 0
}

# 4. Create Six Nodes
for {set i 0} {$i < 6} {incr i} {
    set n($i) [$ns node]
    $n($i) label "Node $i"
    $n($i) shape "circle"
}

# Special visual styling for Source and Destination
$n(0) label "Source (n0)"
$n(0) color "red"
$n(3) label "Destination (n3)"
$n(3) color "darkgreen"

# 5. Create Full Mesh Topology (Interconnect all pairs of nodes)
# For N = 6 nodes, total links = N * (N - 1) / 2 = 15 links
for {set i 0} {$i < 6} {incr i} {
    for {set j [expr $i + 1]} {$j < 6} {incr j} {
        $ns duplex-link $n($i) $n($j) 1.5Mb 10ms DropTail
    }
}

# 6. Configure Ring/Hexagonal Orientation in NAM
$ns duplex-link-op $n(0) $n(1) orient right-up
$ns duplex-link-op $n(1) $n(2) orient right-down
$ns duplex-link-op $n(2) $n(3) orient down
$ns duplex-link-op $n(3) $n(4) orient left-down
$ns duplex-link-op $n(4) $n(5) orient left-up
$ns duplex-link-op $n(5) $n(0) orient up

# 7. Configure UDP Communication with CBR Traffic
set udp [new Agent/UDP]
$udp set class_ 1
$udp set fid_ 1
$ns attach-agent $n(0) $udp

set null [new Agent/Null]
$ns attach-agent $n(3) $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 1024
$cbr set interval_ 0.05
$cbr set random_ false

# 8. Simulate Dynamic Route Discovery & Link Failures
# Start traffic at 0.5s
$ns at 0.5 "$cbr start"

# Fail direct link between n0 and n3 at 2.0s to trigger alternative route discovery
$ns rtmodel-at 2.0 down $n(0) $n(3)
$ns at 2.0 "puts {>>> At 2.0s: Direct link n0-n3 went DOWN! Finding alternate route in mesh...}"

# Restore direct link at 3.5s
$ns rtmodel-at 3.5 up $n(0) $n(3)
$ns at 3.5 "puts {>>> At 3.5s: Direct link n0-n3 came back UP! Reverting to direct route...}"

# Stop traffic at 4.5s and finish at 5.0s
$ns at 4.5 "$cbr stop"
$ns at 5.0 "finish"

# 9. Run Simulation
$ns run

