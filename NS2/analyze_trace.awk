# ==============================================================================
# AWK Script to Analyze NS2 Trace Files (.tr)
# Computes Sent, Received, Dropped Packets, Delivery Ratio & Throughput
#
# Usage:
#   awk -f analyze_trace.awk <trace_file.tr>
# Example:
#   awk -f analyze_trace.awk q1_six_node_wired.tr
# ==============================================================================

BEGIN {
    sent_packets = 0;
    recv_packets = 0;
    drop_packets = 0;
    total_bytes_received = 0;
    start_time = 1e9;
    end_time = 0;
}

{
    action = $1;
    time = $2;
    from_node = $3;
    to_node = $4;
    pkt_type = $5;
    pkt_size = $6;
    fid = $8;

    # Record simulation start and end times
    if (time < start_time) start_time = time;
    if (time > end_time) end_time = time;

    # Count packet events
    if (action == "+" && (from_node == 0 || from_node == 1 || from_node == 2)) {
        sent_packets++;
    }

    if (action == "r") {
        recv_packets++;
        total_bytes_received += pkt_size;
    }

    if (action == "d") {
        drop_packets++;
    }
}

END {
    duration = end_time - start_time;
    if (duration > 0) {
        throughput_kbps = (total_bytes_received * 8) / (duration * 1024);
    } else {
        throughput_kbps = 0;
    }

    if (sent_packets > 0) {
        pdr = (recv_packets / sent_packets) * 100;
    } else {
        pdr = 0;
    }

    print "==================================================";
    print "            NS2 TRACE ANALYSIS REPORT             ";
    print "==================================================";
    printf("Simulation Duration    : %.4f seconds\n", duration);
    printf("Packets Sent           : %d\n", sent_packets);
    printf("Packets Received       : %d\n", recv_packets);
    printf("Packets Dropped        : %d\n", drop_packets);
    printf("Packet Delivery Ratio  : %.2f %%\n", pdr);
    printf("Total Data Received    : %d bytes\n", total_bytes_received);
    printf("Average Throughput     : %.2f Kbps (%.4f Mbps)\n", throughput_kbps, throughput_kbps / 1024);
    print "==================================================";
}

