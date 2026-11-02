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

# 1. Initialize Simulator
set ns [new Simulator]

# Color 1 for UDP/CBR packets (Red)
$ns color 1 Red

# 2. Open Trace Files
set tf [open q4_two_node_udp.tr w]
$ns trace-all $tf

set nf [open q4_two_node_udp.nam w]
$ns namtrace-all $nf

# 3. Finish Procedure
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

# 4. Create Two Nodes
set n0 [$ns node]   ;# UDP Sender
set n1 [$ns node]   ;# UDP Receiver (Null sink)

# Node visual labels for NAM
$n0 label "Node 0 (UDP Source)"
$n1 label "Node 1 (Null Destination)"
$n0 color "red"
$n1 color "magenta"
$n0 shape "circle"
$n1 shape "circle"

# 5. Create Duplex Link
# 2 Mbps bandwidth, 10 ms delay, DropTail queue
$ns duplex-link $n0 $n1 2Mb 10ms DropTail
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

# 6. Configure UDP Agent
set udp [new Agent/UDP]
$udp set class_ 1
$udp set fid_ 1
$ns attach-agent $n0 $udp

# Configure Null Agent (Traffic Sink)
set null [new Agent/Null]
$ns attach-agent $n1 $null

# Connect UDP source to Null sink
$ns connect $udp $null

# 7. Configure CBR Traffic Generator with Exact Required Specifications
set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 1024       ;# 1024 bytes per packet
$cbr set interval_ 0.1          ;# 0.1 seconds between packets
$cbr set random_ false          ;# Fixed deterministic interval

# 8. Schedule Simulation Events
$ns at 0.5 "$cbr start"
$ns at 4.5 "$cbr stop"
$ns at 5.0 "finish"

# 9. Run Simulation
$ns run

