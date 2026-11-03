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

set ns [new Simulator]

$ns color 1 Blue
$ns color 2 Red

set tf [open q3_two_node_tcp.tr w]
$ns trace-all $tf

set nf [open q3_two_node_tcp.nam w]
$ns namtrace-all $nf

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

set n0 [$ns node]
set n1 [$ns node]

$n0 label "Node 0 (TCP Source)"
$n1 label "Node 1 (TCP Sink)"
$n0 color "blue"
$n1 color "navy"
$n0 shape "circle"
$n1 shape "circle"

$ns duplex-link $n0 $n1 1Mb 5ms DropTail

$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

set tcp [new Agent/TCP]
$tcp set class_ 1
$tcp set fid_ 1
$tcp set packetSize_ 1000
$ns attach-agent $n0 $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n1 $sink

$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

$ns at 0.5 "$ftp start"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

$ns run
