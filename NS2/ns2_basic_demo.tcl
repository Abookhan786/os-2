set ns [new Simulator]

set nf [open out.nam w]
$ns namtrace-all $nf

set tf [open out.tr w]
$ns trace-all $tf

proc finish {}{
    global ns nf tf
    
    $ns flush-trace
    close $nf
    close $tf
    
    exec nam out.nam &
    exit 0
}

set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]


$ns duplex-link $n0 $n1 1Mb 10 ms DropTail
$ns duplex-link $n0 $n2 1Mb 10 ms DropTail

#n0 tcp sender
#n1 tcp receiver (sink)

set tcp [new Agent/TCP]
$ns attach-agent $n0 $tcp

set sink [new Agent/TCPSink]
$ns attach-agent $n1 $sink

$ns connect $tcp $sink


#create ftp 

set ftp [new Application/FTP]
$ns attach-agent $tcp

$ns at 1.0 "$ftp start"
$ns at 5.0 "$ftp stop" 


#n1 udp sender
#n2 udp receiver

set udp [new Agent/UDP]
$ns attach-agent $n1 $udp

set null [new Agent/Null]
$ns attach-agent $n2 $null

$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
$cbr set packetSize_ 500
$cbr set interval_ 0.01

$ns attach-agent $udp

$ns at 0.5 "$cbr start"
$ns at 4.5 "$cbr stop"

$ns at 10.0 "finish"
$ns run
