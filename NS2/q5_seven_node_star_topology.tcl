# ==============================================================================
# Question 5: 7-Node Star Topology Network in NS2
# ==============================================================================
# Description:
#   - Constructs a 7-node network arranged in a Star Topology.
#   - Node n0 acts as the Central Hub/Switch.
#   - Nodes n1, n2, n3, n4, n5, n6 act as Peripheral (End/Spoke) Nodes.
#   - Traffic is configured from an end node (e.g., n1) to another end node
#     (e.g., n4) routed through the central node n0.
#   - A second traffic flow is configured from n2 to n5 through n0.
#   - Demonstrates packet forwarding, queuing, and hub-and-spoke dynamics in NAM.
#
# How to Run:
#   ns q5_seven_node_star_topology.tcl
# ==============================================================================

# 1. Initialize Simulator
set ns [new Simulator]

# Color coding for NAM visualization
$ns color 1 Blue       ;# Flow 1: TCP/FTP from n1 to n4 via n0
$ns color 2 Red        ;# Flow 2: UDP/CBR from n2 to n5 via n0

# 2. Open Trace Files
set tf [open q5_star_topology.tr w]
$ns trace-all $tf

set nf [open q5_star_topology.nam w]
$ns namtrace-all $nf

# 3. Finish Procedure
proc finish {} {
    global ns tf nf
    $ns flush-trace
    close $tf
    close $nf
    puts "=========================================================="
    puts "   7-Node Star Topology Simulation Finished               "
    puts "=========================================================="
    puts "Central Hub Node : Node 0 (n0)"
    puts "Peripheral Nodes : Nodes 1 through 6 (n1 to n6)"
    puts "Flow 1 Route     : n1 -> n0 (Hub) -> n4"
    puts "Flow 2 Route     : n2 -> n0 (Hub) -> n5"
    puts "Trace File       : q5_star_topology.tr"
    puts "NAM File         : q5_star_topology.nam"
    puts "Launching NAM..."
    exec nam q5_star_topology.nam &
    exit 0
}

# 4. Create 7 Nodes (Central Hub + 6 Peripheral Spoke Nodes)
set n0 [$ns node]   ;# Central Hub
set n1 [$ns node]   ;# End Node 1
set n2 [$ns node]   ;# End Node 2
set n3 [$ns node]   ;# End Node 3
set n4 [$ns node]   ;# End Node 4
set n5 [$ns node]   ;# End Node 5
set n6 [$ns node]   ;# End Node 6

# Styling and Labels for NAM
$n0 label "Central Hub (n0)"
$n0 color "darkgreen"
$n0 shape "box"

for {set i 1} {$i <= 6} {incr i} {
    set node_var [set n$i]
    $node_var label "End Node $i"
    $node_var color "royalblue"
    $node_var shape "circle"
}

# 5. Create Star Links (Every peripheral node connects ONLY to Central Hub n0)
# Syntax: $ns duplex-link <spoke> <hub> <bandwidth> <delay> <queue>
$ns duplex-link $n1 $n0 2Mb 10ms DropTail
$ns duplex-link $n2 $n0 2Mb 10ms DropTail
$ns duplex-link $n3 $n0 2Mb 10ms DropTail
$ns duplex-link $n4 $n0 2Mb 10ms DropTail
$ns duplex-link $n5 $n0 2Mb 10ms DropTail
$ns duplex-link $n6 $n0 2Mb 10ms DropTail

# 6. Configure Star Layout Orientations in NAM (Radially around n0)
$ns duplex-link-op $n0 $n1 orient up
$ns duplex-link-op $n0 $n2 orient right-up
$ns duplex-link-op $n0 $n3 orient right-down
$ns duplex-link-op $n0 $n4 orient down
$ns duplex-link-op $n0 $n5 orient left-down
$ns duplex-link-op $n0 $n6 orient left-up

# Queue monitoring on central hub links
$ns duplex-link-op $n0 $n1 queuePos 0.5
$ns duplex-link-op $n0 $n4 queuePos 0.5

# 7. Flow 1: End Node n1 to End Node n4 through Central Hub n0 (TCP/FTP)
set tcp [new Agent/TCP]
$tcp set class_ 1
$tcp set fid_ 1
$ns attach-agent $n1 $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n4 $sink

$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

# 8. Flow 2: End Node n2 to End Node n5 through Central Hub n0 (UDP/CBR)
set udp [new Agent/UDP]
$udp set class_ 2
$udp set fid_ 2
$ns attach-agent $n2 $udp

set null [new Agent/Null]
$ns attach-agent $n5 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 512
$cbr set rate_ 1Mb
$cbr set random_ false

# 9. Schedule Simulation Events
$ns at 0.5 "$ftp start"
$ns at 1.0 "$cbr start"
$ns at 4.0 "$cbr stop"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

# 10. Run Simulation
$ns run

