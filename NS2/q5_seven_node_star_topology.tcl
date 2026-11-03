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

set ns [new Simulator]

$ns color 1 Blue
$ns color 2 Red

set tf [open q5_star_topology.tr w]
$ns trace-all $tf

set nf [open q5_star_topology.nam w]
$ns namtrace-all $nf

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

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
set n6 [$ns node]

$n0 label "Central Hub (n0)"
$n0 color "darkgreen"
$n0 shape "box"

for {set i 1} {$i <= 6} {incr i} {
    set node_var [set n$i]
    $node_var label "End Node $i"
    $node_var color "royalblue"
    $node_var shape "circle"
}

$ns duplex-link $n1 $n0 2Mb 10ms DropTail
$ns duplex-link $n2 $n0 2Mb 10ms DropTail
$ns duplex-link $n3 $n0 2Mb 10ms DropTail
$ns duplex-link $n4 $n0 2Mb 10ms DropTail
$ns duplex-link $n5 $n0 2Mb 10ms DropTail
$ns duplex-link $n6 $n0 2Mb 10ms DropTail

$ns duplex-link-op $n0 $n1 orient up
$ns duplex-link-op $n0 $n2 orient right-up
$ns duplex-link-op $n0 $n3 orient right-down
$ns duplex-link-op $n0 $n4 orient down
$ns duplex-link-op $n0 $n5 orient left-down
$ns duplex-link-op $n0 $n6 orient left-up

$ns duplex-link-op $n0 $n1 queuePos 0.5
$ns duplex-link-op $n0 $n4 queuePos 0.5

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

$ns at 0.5 "$ftp start"
$ns at 1.0 "$cbr start"
$ns at 4.0 "$cbr stop"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

$ns run
