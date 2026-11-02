# ==============================================================================
# Question 1: Six-Node Wired Network in NS2 with Different Link Configurations
# ==============================================================================
# Description:
#   - Designs a 6-node wired network (n0 to n5) in a dumbbell/multi-hop topology.
#   - Uses different link configurations (varying bandwidths: 2Mb, 1.5Mb, 1Mb,
#     512Kb and propagation delays: 10ms, 20ms, 30ms, 15ms).
#   - Implements bottleneck link between routers n2 and n3 with limited queue.
#   - Establishes TCP communication (n0 -> n4) and UDP communication (n1 -> n5).
#   - Visualizes packet flow, queue buildup, and drops in NAM.
#
# How to Run:
#   ns q1_six_node_wired.tcl
# ==============================================================================

# 1. Initialize Simulator
set ns [new Simulator]

# Define packet flow colors for NAM visualization
$ns color 1 Blue     ;# Flow 1: TCP/FTP traffic (n0 -> n4)
$ns color 2 Red      ;# Flow 2: UDP/CBR traffic (n1 -> n5)

# 2. Open Trace Files
set tf [open q1_six_node_wired.tr w]
$ns trace-all $tf

set nf [open q1_six_node_wired.nam w]
$ns namtrace-all $nf

# 3. Finish Procedure
proc finish {} {
    global ns tf nf
    $ns flush-trace
    close $tf
    close $nf
    puts "Simulation completed successfully."
    puts "Trace file: q1_six_node_wired.tr"
    puts "NAM file  : q1_six_node_wired.nam"
    exec nam q1_six_node_wired.nam &
    exit 0
}

# 4. Create Six Nodes
set n0 [$ns node]   ;# Source 1 (TCP)
set n1 [$ns node]   ;# Source 2 (UDP)
set n2 [$ns node]   ;# Intermediate Gateway/Router 1
set n3 [$ns node]   ;# Intermediate Gateway/Router 2
set n4 [$ns node]   ;# Destination 1 (TCP Sink)
set n5 [$ns node]   ;# Destination 2 (UDP Null Sink)

# Set node labels and colors for NAM
$n0 label "Source 0 (TCP)"
$n1 label "Source 1 (UDP)"
$n2 label "Router 1"
$n3 label "Router 2"
$n4 label "Dest 4 (Sink)"
$n5 label "Dest 5 (Null)"

$n0 color "blue"
$n1 color "red"
$n2 color "brown"
$n3 color "brown"
$n4 color "blue"
$n5 color "red"

# 5. Create Duplex Links with DIFFERENT Configurations
# Syntax: $ns duplex-link <node1> <node2> <bandwidth> <delay> <queue_type>

# Link n0 - n2: 2 Mbps, 10 ms delay, DropTail queue
$ns duplex-link $n0 $n2 2Mb 10ms DropTail

# Link n1 - n2: 1.5 Mbps, 20 ms delay, DropTail queue
$ns duplex-link $n1 $n2 1.5Mb 20ms DropTail

# Link n2 - n3 (Core / Bottleneck): 1 Mbps, 30 ms delay, DropTail queue
$ns duplex-link $n2 $n3 1Mb 30ms DropTail

# Link n3 - n4: 2 Mbps, 10 ms delay, DropTail queue
$ns duplex-link $n3 $n4 2Mb 10ms DropTail

# Link n3 - n5: 512 Kbps, 15 ms delay, DropTail queue
$ns duplex-link $n3 $n5 512Kb 15ms DropTail

# Set queue limit on bottleneck link
$ns queue-limit $n2 $n3 10

# 6. Configure Link Layouts for NAM Display
$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n1 $n2 orient right-up
$ns duplex-link-op $n2 $n3 orient right
$ns duplex-link-op $n3 $n4 orient right-up
$ns duplex-link-op $n3 $n5 orient right-down

# Display queue on bottleneck link
$ns duplex-link-op $n2 $n3 queuePos 0.5

# 7. Configure Flow 1: TCP with FTP (n0 -> n4)
set tcp [new Agent/TCP]
$tcp set class_ 1
$tcp set fid_ 1
$ns attach-agent $n0 $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n4 $sink

$ns connect $tcp $sink

set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

# 8. Configure Flow 2: UDP with CBR (n1 -> n5)
set udp [new Agent/UDP]
$udp set class_ 2
$udp set fid_ 2
$ns attach-agent $n1 $udp

set null [new Agent/Null]
$ns attach-agent $n5 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set packetSize_ 1000
$cbr set rate_ 500Kb
$cbr set random_ false

# 9. Schedule Simulation Events
$ns at 0.5 "$ftp start"
$ns at 1.0 "$cbr start"
$ns at 4.0 "$cbr stop"
$ns at 4.5 "$ftp stop"
$ns at 5.0 "finish"

# 10. Run Simulation
$ns run

