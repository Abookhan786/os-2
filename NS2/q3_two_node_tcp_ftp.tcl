# ==============================================================================
# Question 3: Two-Node Network with TCP/FTP (1 Mbps Bandwidth, 5 ms Delay)
# ==============================================================================
# Description:
#   - Creates a 2-node point-to-point network (n0 and n1).
#   - Configures a duplex link with:
#       * Bandwidth         : 1 Mbps (1Mb)
#       * Propagation Delay : 5 ms (5ms)
#       * Queue Management  : DropTail
#   - Attaches a TCP agent to n0 and TCPSink to n1.
#   - Sets up an FTP application over TCP to generate file transfer traffic.
#   - Visualizes packet exchange and acknowledgments using NAM.
#
# How to Run:
#   ns q3_two_node_tcp_ftp.tcl
# ==============================================================================

# 1. Initialize Simulator
set ns [new Simulator]

# Color 1 for TCP data packets (Blue), Color 2 for ACKs (Red)
$ns color 1 Blue
$ns color 2 Red

# 2. Open Trace Files
set tf [open q3_two_node_tcp.tr w]
$ns trace-all $tf

set nf [open q3_two_node_tcp.nam w]
$ns namtrace-all $nf

# 3. Finish Procedure
proc finish {} {
    global ns tf nf
    $ns flush-trace
    close $tf
    close $nf
    puts "=========================================================="
    puts "   Two-Node TCP/FTP Simulation Finished                   "
    puts "=========================================================="
    puts "Link Parameters  : Bandwidth = 1 Mbps, Delay = 5 ms"
    puts "Trace File       : q3_two_node_tcp.tr"
    puts "NAM Trace File   : q3_two_node_tcp.nam"
    puts "Launching NAM for visualization..."
    exec nam q3_two_node_tcp.nam &
    exit 0
}

# 4. Create Two Nodes
set n0 [$ns node]   ;# Source Node
set n1 [$ns node]   ;# Destination Node

# Labels and Styling
$n0 label "Node 0 (TCP Source)"
$n1 label "Node 1 (TCP Sink)"
$n0 color "blue"
$n1 color "navy"
$n0 shape "circle"
$n1 shape "circle"

# 5. Create Duplex Link with Specified Link Parameters
# Exactly 1 Mbps bandwidth, 5 ms delay as requested in problem statement
$ns duplex-link $n0 $n1 1Mb 5ms DropTail

# Configure orientation and queue display for NAM
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

# 6. Configure TCP Agent
set tcp [new Agent/TCP]
$tcp set class_ 1
$tcp set fid_ 1
$tcp set packetSize_ 1000       ;# Standard 1000-byte packets
$ns attach-agent $n0 $tcp

# Configure TCP Sink (Receiver)
set sink [new Agent/TCPSink]
$ns attach-agent $n1 $sink

# Connect TCP source to sink
$ns connect $tcp $sink

# 7. Configure FTP Application
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

# 8. Schedule Simulation Events
$ns at 0.5 "$ftp start"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

# 9. Run Simulation
$ns run

