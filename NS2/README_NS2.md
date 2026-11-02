# NS2 Network Simulation Lab Programs

Complete implementations of all 6 Network Simulation 2 (NS2) lab questions with NAM visualization and trace recording.

---

## 📁 Program Index

| Question | TCL Script | Output Trace (`.tr`) | NAM Animation (`.nam`) | Key Concepts |
|---|---|---|---|---|
| **Q1** | [`q1_six_node_wired.tcl`](q1_six_node_wired.tcl) | `q1_six_node_wired.tr` | `q1_six_node_wired.nam` | 6-node dumbbell topology, differing bandwidths & delays, bottleneck queues, TCP + UDP traffic |
| **Q2** | [`q2_tcp_ftp_file_transfer.tcl`](q2_tcp_ftp_file_transfer.tcl) | `q2_tcp_ftp.tr` | `q2_tcp_ftp.nam` | TCP file transfer with FTP, CWND tracking (`q2_cwnd.data`), Slow Start & Congestion Avoidance |
| **Q3** | [`q3_two_node_tcp_ftp.tcl`](q3_two_node_tcp_ftp.tcl) | `q3_two_node_tcp.tr` | `q3_two_node_tcp.nam` | 2-node point-to-point network with **1 Mbps** bandwidth and **5 ms** propagation delay |
| **Q4** | [`q4_two_node_udp_cbr.tcl`](q4_two_node_udp_cbr.tcl) | `q4_two_node_udp.tr` | `q4_two_node_udp.nam` | 2-node UDP network with CBR traffic, packet size **1024 bytes**, interval **0.1 seconds** |
| **Q5** | [`q5_seven_node_star_topology.tcl`](q5_seven_node_star_topology.tcl) | `q5_star_topology.tr` | `q5_star_topology.nam` | 7-node Star Topology with central hub `n0` and 6 spokes `n1-n6`, packet forwarding via hub |
| **Q6** | [`q6_six_node_mesh_topology.tcl`](q6_six_node_mesh_topology.tcl) | `q6_mesh_topology.tr` | `q6_mesh_topology.nam` | Full 6-node Mesh (15 duplex links), Distance Vector (`rtproto DV`), dynamic rerouting around link failure |

---

## 🚀 How to Run the Simulations

Open your terminal (Linux / Ubuntu / WSL) where NS2 is installed:

### Question 1: Six-Node Wired Network
```bash
ns q1_six_node_wired.tcl
```

### Question 2: TCP File Transfer with FTP
```bash
ns q2_tcp_ftp_file_transfer.tcl
```
*(Optionally view CWND graph using gnuplot: `gnuplot -e "plot 'q2_cwnd.data' using 1:2 with lines title 'CWND'; pause -1"`)*

### Question 3: Two-Node TCP/FTP (1 Mbps, 5 ms)
```bash
ns q3_two_node_tcp_ftp.tcl
```

### Question 4: Two-Node UDP/CBR (1024B, 0.1s interval)
```bash
ns q4_two_node_udp_cbr.tcl
```

### Question 5: 7-Node Star Topology
```bash
ns q5_seven_node_star_topology.tcl
```

### Question 6: 6-Node Mesh Topology
```bash
ns q6_six_node_mesh_topology.tcl
```

---

## 📊 How to Analyze Trace Files with AWK

An analysis script [`analyze_trace.awk`](analyze_trace.awk) is provided to calculate total sent/received packets, drops, packet delivery ratio (PDR), and throughput:

```bash
awk -f analyze_trace.awk q1_six_node_wired.tr
awk -f analyze_trace.awk q2_tcp_ftp.tr
awk -f analyze_trace.awk q3_two_node_tcp.tr
awk -f analyze_trace.awk q4_two_node_udp.tr
awk -f analyze_trace.awk q5_star_topology.tr
awk -f analyze_trace.awk q6_mesh_topology.tr
```

