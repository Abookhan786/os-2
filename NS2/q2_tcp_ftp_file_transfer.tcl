# ==============================================================================
# Question 2: Simulate TCP-Based File Transfer in NS2 using FTP Traffic
# ==============================================================================
# Description:
#   - Simulates end-to-end file transfer between two nodes using TCP and FTP.
#   - Records transmission behavior including TCP congestion window (cwnd)
#     growth over time.
#   - Demonstrates TCP Slow Start and Congestion Avoidance phases.
#   - Generates trace files (q2_tcp_ftp.tr, q2_tcp_ftp.nam, q2_cwnd.data)
#     to record and examine transmission dynamics.
#
# How to Run:
#   ns q2_tcp_ftp_file_transfer.tcl
# ==============================================================================

# 1. Initialize Simulator
set ns [new Simulator]

# Define color coding for TCP traffic
$ns color 1 Blue

# 2. Open Output Trace Files
set tf [open q2_tcp_ftp.tr w]
$ns trace-all $tf

set nf [open q2_tcp_ftp.nam w]
$ns namtrace-all $nf

# Open data file to record TCP Congestion Window (cwnd)
set cwnd_file [open q2_cwnd.data w]

# 3. Finish Procedure
proc finish {} {
    global ns tf nf cwnd_file
    $ns flush-trace
    close $tf
    close $nf
    close $cwnd_file

    puts "=========================================================="
    puts "   TCP File Transfer Simulation Completed Successfully   "
    puts "=========================================================="
    puts "Trace file generated : q2_tcp_ftp.tr"
    puts "NAM file generated   : q2_tcp_ftp.nam"
    puts "CWND log file        : q2_cwnd.data"
    puts "----------------------------------------------------------"
    puts "You can view the congestion window data in 'q2_cwnd.data'"
    puts "or visualize the simulation with NAM."
    puts "=========================================================="

    exec nam q2_tcp_ftp.nam &
    exit 0
}

# 4. Create Nodes
set n0 [$ns node]   ;# Server / Sender
set n1 [$ns node]   ;# Client / Receiver

# Labels and visual styling for NAM
$n0 label "FTP Server (Sender)"
$n1 label "FTP Client (Receiver)"
$n0 color "blue"
$n1 color "green"
$n0 shape "box"
$n1 shape "box"

# 5. Create Duplex Link
# 2 Mbps bandwidth, 10 ms propagation delay, DropTail queue
$ns duplex-link $n0 $n1 2Mb 10ms DropTail
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

# 6. Configure TCP Agent (TCP Reno)
set tcp [new Agent/TCP/Reno]
$tcp set class_ 1
$tcp set fid_ 1
$tcp set packetSize_ 1024       ;# 1 KB packet size for file transfer
$tcp set window_ 32             ;# Maximum advertised window
$ns attach-agent $n0 $tcp

# Configure TCP Sink (Receiver)
set sink [new Agent/TCPSink]
$ns attach-agent $n1 $sink

# Connect TCP source to sink
$ns connect $tcp $sink

# 7. Configure FTP Application (File Transfer Generator)
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP

# 8. Procedure to Record CWND Over Time (Examine Transmission Behavior)
proc record_cwnd {} {
    global ns tcp cwnd_file
    set time_interval 0.1
    set now [$ns now]
    set cwnd [$tcp set cwnd_]
    set rtt [$tcp set rtt_]

    # Write: <Time> <Congestion_Window> <Estimated_RTT>
    puts $cwnd_file "$now $cwnd $rtt"

    # Reschedule next measurement
    $ns at [expr $now + $time_interval] "record_cwnd"
}

# 9. Schedule Simulation Events
$ns at 0.0 "record_cwnd"
$ns at 1.0 "$ftp start"
puts "Starting FTP file transfer at 1.0s..."
$ns at 9.0 "$ftp stop"
puts "Stopping FTP file transfer at 9.0s..."
$ns at 10.0 "finish"

# 10. Run Simulation
$ns run

