# ==============================================================================
# Question 4: Two-Node Network with UDP/CBR (Packet Size 1024B, Interval 0.1s)
# ==============================================================================
# Description:
#   - Builds a 2-node point-to-point network (n0 and n1).
#   - Establishes a UDP connection between n0 (Agent/UDP) and n1 (Agent/Null).
#   - Configures a Constant Bit Rate (CBR) traffic generator with:
#       * Packet Size : 1024 bytes
#       * Interval    : 0.1 seconds (10 packets/second)
#   - Runs the simulation and launches NAM to observe UDP packet flow.
#
# How to Run:
#   ns q4_two_node_udp_cbr.tcl
# ==============================================================================

set ns [new Simulator]

$ns color 1 Red

set tf [open q4_two_node_udp.tr w]
$ns trace-all $tf

set nf [open q4_two_node_udp.nam w]
$ns namtrace-all $nf

proc finish {} {
    global ns tf nf
    $ns flush-trace
    close $tf
    close $nf
    puts "=========================================================="
    puts "   Two-Node UDP/CBR Simulation Finished                  "
    puts "=========================================================="
    puts "Configured Parameters:"
    puts "  - Packet Size : 1024 bytes"
    puts "  - Interval    : 0.1 seconds"
    puts "Trace File      : q4_two_node_udp.tr"
    puts "NAM Trace File  : q4_two_node_udp.nam"
    puts "Opening NAM visualization..."
    exec nam q4_two_node_udp.nam &
    exit 0
}

set n0 [$ns node]
set n1 [$ns node]

$n0 label "Node 0 (UDP Source)"
$n1 label "Node 1 (Null Destination)"
$n0 color "red"
$n1 color "magenta"
$n0 shape "circle"
$n1 shape "circle"

$ns duplex-link $n0 $n1 2Mb 10ms DropTail
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

set udp [new Agent/UDP]
$udp set class_ 1
$udp set fid_ 1
$ns attach-agent $n0 $udp

set null [new Agent/Null]
$ns attach-agent $n1 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 1024
$cbr set interval_ 0.1
$cbr set random_ false

$ns at 0.5 "$cbr start"
$ns at 4.5 "$cbr stop"
$ns at 5.0 "finish"

$ns run
