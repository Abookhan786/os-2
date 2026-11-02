# Experiment 4a: UDP Socket Programming in C

---

## 1. Overview & Theoretical Background

### Why UDP (User Datagram Protocol)?
Unlike **TCP**, which is connection-oriented, performs a 3-way handshake (`SYN`, `SYN-ACK`, `ACK`), and provides guaranteed delivery through acknowledgments and retransmissions, **UDP (User Datagram Protocol)** is:
1. **Connectionless:** No connection establishment or teardown overhead. Datagrams are transmitted immediately.
2. **Minimal Latency & Overhead:** UDP header is only **8 bytes** (vs TCP's minimum 20 bytes).
3. **No Head-of-Line Blocking:** A dropped packet does not stall subsequent data.
4. **Ideal for Real-Time Telemetry & Tracking:**
   - **Scenario 1 (Drone Tracking):** Old location data is obsolete once a new coordinate is generated. Re-transmitting old coordinates creates lag, which could cause a drone to crash.
   - **Scenario 2 (Factory Monitoring):** Industrial condition monitoring prioritizes instant notification of critical spikes (overheating, vibration) over guaranteed historical receipt.

---

## 2. Source Code Files in this Directory

| Part | File Name | Description |
| :--- | :--- | :--- |
| **Chat Program** | `ex4a_chat_server.c` | Two-way interactive UDP Chat Server |
| | `ex4a_chat_client.c` | Two-way interactive UDP Chat Client |
| **Scenario 1: Drone** | `station_receiver.c` | Ground Station UDP telemetry receiver & dashboard |
| | `drone_sender.c` | Drone onboard UDP telemetry transmitter |
| **Scenario 2: Factory** | `control_room_receiver.c` | Factory Control Room UDP receiver with safety alerts |
| | `factory_sender.c` | Factory Machine sensor UDP telemetry transmitter |

---

## 3. Compilation Commands

### On Windows (MinGW GCC):
Compile each program using GCC:
```cmd
gcc ex4a_chat_server.c -o ex4a_chat_server.exe -lws2_32 -Wall
gcc ex4a_chat_client.c -o ex4a_chat_client.exe -lws2_32 -Wall
gcc station_receiver.c -o station_receiver.exe -lws2_32 -Wall
gcc drone_sender.c -o drone_sender.exe -lws2_32 -Wall
gcc control_room_receiver.c -o control_room_receiver.exe -lws2_32 -Wall
gcc factory_sender.c -o factory_sender.exe -lws2_32 -Wall
```

### On Linux / Ubuntu:
```bash
gcc ex4a_chat_server.c -o ex4a_chat_server -Wall
gcc ex4a_chat_client.c -o ex4a_chat_client -Wall
gcc station_receiver.c -o station_receiver -Wall
gcc drone_sender.c -o drone_sender -Wall
gcc control_room_receiver.c -o control_room_receiver -Wall
gcc factory_sender.c -o factory_sender -Wall
```

---

## 4. Execution & Sample Outputs

### Part 0: Two-Way UDP Chat Program
Open two terminal windows:

**Terminal 1 (Server):**
```cmd
.\ex4a_chat_server.exe
```

**Terminal 2 (Client):**
```cmd
.\ex4a_chat_client.exe 127.0.0.1
```

#### Sample Interaction:
**Server Terminal:**
```text
====================================================
        UDP CHAT SERVER STARTED (Port: 8888)          
  Waiting for client message... (Type 'exit' to quit)
====================================================

[Client 127.0.0.1:52134]: Hello Server!
Server: Hello Client! How can I assist you?
[Client 127.0.0.1:52134]: Requesting status report.
Server: All systems operational.
[Client 127.0.0.1:52134]: exit

[Server]: Client has left the chat. Shutting down...
Server socket closed.
```

**Client Terminal:**
```text
====================================================
        UDP CHAT CLIENT CONNECTED TO 127.0.0.1:8888          
       Type message and press Enter ('exit' to quit)
====================================================

Client: Hello Server!
[Server]: Hello Client! How can I assist you?
Client: Requesting status report.
[Server]: All systems operational.
Client: exit

[Client]: You ended the chat session.
Client socket closed.
```

---

### Scenario 1: Real-Time Drone Telemetry Tracking

**Terminal 1 (Ground Station Receiver):**
```cmd
.\station_receiver.exe
```

**Terminal 2 (Drone Sender):**
```cmd
.\drone_sender.exe 127.0.0.1 5
```

#### Output:
**Drone Sender:**
```text
======================================================================
         DRONE ONBOARD TELEMETRY TRANSMITTER (UDP CLIENT)             
  Target Station: 127.0.0.1:5005 | Telemetry Stream: 5 packets               
======================================================================

[TX #01] Lat: 13.083130, Lon: 80.271038, Alt: 43.0m, Spd: 31.0km/h, Batt: 99% (56 bytes sent)
[TX #02] Lat: 13.083580, Lon: 80.271358, Alt: 48.5m, Spd: 29.5km/h, Batt: 98% (56 bytes sent)
[TX #03] Lat: 13.084030, Lon: 80.271678, Alt: 46.5m, Spd: 32.5km/h, Batt: 97% (56 bytes sent)
[TX #04] Lat: 13.084480, Lon: 80.271998, Alt: 52.0m, Spd: 31.0km/h, Batt: 96% (56 bytes sent)
[TX #05] Lat: 13.084930, Lon: 80.272318, Alt: 50.0m, Spd: 29.5km/h, Batt: 95% (56 bytes sent)

[DRONE]: Telemetry stream completed. Drone landed.
```

**Station Receiver:**
```text
========================================================================================
                DRONE GROUND MONITORING STATION (UDP RECEIVER)                          
                Listening on Port: 5005 | Protocol: UDP (Lightweight)                    
========================================================================================
Seq#  | Drone ID     | Latitude    | Longitude   | Altitude(m)  | Speed(km/h) | Batt %  | Status    
------+--------------+-------------+-------------+--------------+-------------+---------+-----------
1     | DRONE-ALPHA1 |   13.083130 |   80.271038 |        43.00 |       31.00 |     99% | OPTIMAL   
2     | DRONE-ALPHA1 |   13.083580 |   80.271358 |        48.50 |       29.50 |     98% | OPTIMAL   
3     | DRONE-ALPHA1 |   13.084030 |   80.271678 |        46.50 |       32.50 |     97% | OPTIMAL   
4     | DRONE-ALPHA1 |   13.084480 |   80.271998 |        52.00 |       31.00 |     96% | OPTIMAL   
5     | DRONE-ALPHA1 |   13.084930 |   80.272318 |        50.00 |       29.50 |     95% | OPTIMAL   
------+--------------+-------------+-------------+--------------+-------------+---------+-----------
[STATION]: Drone 'DRONE-ALPHA1' landed / finished transmission.
========================================================================================
Monitoring session closed.
```

---

### Scenario 2: Factory Machine Real-Time Monitoring Unit

**Terminal 1 (Control Room Receiver):**
```cmd
.\control_room_receiver.exe
```

**Terminal 2 (Factory Sensor Sender):**
```cmd
.\factory_sender.exe 127.0.0.1 6
```

#### Output:
**Factory Machine Sensor Unit:**
```text
======================================================================
        FACTORY SENSOR TELEMETRY UNIT (UDP TRANSMITTER)               
  Target Control Room: 127.0.0.1:6006 | Reports to send: 6                   
======================================================================

[TX #01] Machine #402 (CNC-Hydraulic-Press) -> Temp: 64.5 C, Press: 4.05 Bar, Vib: 1.78 mm/s, RPM: 2370 (56 bytes)
[TX #02] Machine #402 (CNC-Hydraulic-Press) -> Temp: 67.0 C, Press: 4.40 Bar, Vib: 2.16 mm/s, RPM: 2420 (56 bytes)
[TX #03] Machine #402 (CNC-Hydraulic-Press) -> Temp: 69.5 C, Press: 4.25 Bar, Vib: 2.54 mm/s, RPM: 2390 (56 bytes)
[TX #04] Machine #402 (CNC-Hydraulic-Press) -> Temp: 72.0 C, Press: 4.60 Bar, Vib: 2.92 mm/s, RPM: 2440 (56 bytes)
[TX #05] Machine #402 (CNC-Hydraulic-Press) -> Temp: 74.5 C, Press: 4.45 Bar, Vib: 3.30 mm/s, RPM: 2410 (56 bytes)
[TX #06] Machine #402 (CNC-Hydraulic-Press) -> Temp: 77.0 C, Press: 4.80 Bar, Vib: 3.68 mm/s, RPM: 2460 (56 bytes)

[FACTORY UNIT]: Monitoring transmission finished.
```

**Control Room Receiver:**
```text
================================================================================================
                  CENTRAL FACTORY CONTROL ROOM - REAL-TIME MONITORING                          
                    Listening on UDP Port: 6006 | Low-Latency Telemetry                          
================================================================================================
Seq#  | M-ID   | Machine Name         | Temp(C)   | Press(Bar) | Vib(mm/s)   | RPM     | Safety State      
------+--------+----------------------+-----------+------------+-------------+---------+------------------
1     | #402   | CNC-Hydraulic-Press  |    64.5 C |    4.05 Bar |     1.78 mm/s |   2370 | [NORMAL]          
2     | #402   | CNC-Hydraulic-Press  |    67.0 C |    4.40 Bar |     2.16 mm/s |   2420 | [NORMAL]          
3     | #402   | CNC-Hydraulic-Press  |    69.5 C |    4.25 Bar |     2.54 mm/s |   2390 | [NORMAL]          
4     | #402   | CNC-Hydraulic-Press  |    72.0 C |    4.60 Bar |     2.92 mm/s |   2440 | [NORMAL]          
5     | #402   | CNC-Hydraulic-Press  |    74.5 C |    4.45 Bar |     3.30 mm/s |   2410 | [NORMAL]          
6     | #402   | CNC-Hydraulic-Press  |    77.0 C |    4.80 Bar |     3.68 mm/s |   2460 | * WARNING HIGH *  
------+--------+----------------------+-----------+------------+-------------+---------+------------------
[CONTROL ROOM]: Sensor telemetry stream ended by Machine #402.
================================================================================================
Factory Control Room monitoring closed.
```
