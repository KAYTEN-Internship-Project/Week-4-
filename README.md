# CAN ↔ UDP Gateway on AURIX TC375 Lite

This repository provides firmware and documentation for implementing a **bidirectional CAN ↔ UDP gateway** on the **Infineon AURIX™ TC375 Lite Kit**.  
The project enables seamless communication between a CAN bus and Ethernet (UDP/IP), bridging automotive communication with IP-based systems.

![WhatsApp Image 2025-09-10 at 17 17 19](https://github.com/user-attachments/assets/61c5fb90-4087-4b35-a11b-c15d6780d80d)

*Photo: Test setup including TC375 Lite boards, PEAK CAN-USB interface, and wiring connections.*

---

## ✨ Features

- Bidirectional conversion: **CAN → UDP** and **UDP → CAN**.  
- Based on **Infineon AURIX TC375 Lite Kit** with **TLE9251V CAN transceiver**.  
- Configurable **MAC** and **IP** addresses in `bridge_common.h`.  
- Uses **lwIP stack** for UDP communication.  
- Verified with:
  - **PEAK PCAN-USB FD Pro**
  - **Saleae Logic Analyzer**
  - **PCAN-View software**
  - **Wireshark** (Ethernet monitoring)  
- Demonstrated transmission of **CAN ID 0x300h** at 500 kbit/s.  

---

## 📡 System Overview

- **Board A (CAN → UDP):** Receives CAN frames and forwards them as UDP packets.  
- **Board B (UDP → CAN):** Receives UDP packets and retransmits them on the CAN bus.  
- Both boards communicate over **UDP port 20000**.  
- PC with PEAK PCAN-USB FD Pro monitors CAN messages.  
- Saleae Logic Analyzer validates low-level signals.  

---

## 🖼 Hardware Setup

### DB9 CAN Pinout
<img width="600" height="250" alt="Pin_out_gösterilmesi" src="https://github.com/user-attachments/assets/4872af62-2c96-49b5-8ef9-1dd641841882" />

*Standard DB9 pinout used by PEAK CAN adapters. Pin 2 = CAN Low, Pin 3 = CAN Ground, Pin 7 = CAN High.*

- Pin 2 → **CAN_L**  
- Pin 3 → **CAN_GND**  
- Pin 7 → **CAN_H**  

---

### Network Topology
<img width="947" height="135" alt="PCAN_!20OHM" src="https://github.com/user-attachments/assets/44620f4c-60cf-485c-a9cf-4541f8ca3efc" />

*Typical CAN bus wiring: PC connected through PEAK PCAN-USB Pro, terminated with 120 Ω resistors on both ends.*

- 120 Ω termination resistors at both ends.  
- PC ↔ PCAN-USB ↔ TC375 boards (CAN_H, CAN_L, GND).  

---

### PEAK CAN USB FD Pro
<img width="273" height="547" alt="Peak_Can_USB_fd_pro" src="https://github.com/user-attachments/assets/104e6a4a-a057-48b5-bb3c-e77961cc0751" />

*PEAK PCAN-USB FD Pro adapter used for monitoring and transmitting CAN messages at 500 kbit/s.*

---

### Saleae Logic Analyzer Pinout
<img width="309" height="234" alt="Saleae_Pinout" src="https://github.com/user-attachments/assets/e689af6d-c52f-4074-88a1-f63e8e69cd41" />

*Saleae logic analyzer pinout for capturing CAN_H and CAN_L signals, verifying bit timing and frame integrity.*

---

### TC375 CAN Transceiver
<img width="207" height="285" alt="Screenshot 2025-09-10 151535" src="https://github.com/user-attachments/assets/3b46231f-6e8e-4450-af17-13077bf2c9be" />

*On-board Infineon TLE9251V CAN transceiver used on the TC375 Lite Kit to interface with the CAN bus.*

---

## 💻 Code Explanation

The firmware consists of the following modules:

- **`can_hal.c / .h`**  
  Hardware abstraction layer for CAN communication. Handles initialization, message transmission, and reception with configurable baud rate (500 kbit/s by default).

- **`udp_bridge.c / .h`**  
  Wrapper around lwIP UDP functions. Provides a clean API for sending and receiving CAN frames over UDP, with callback support.

- **`bridge_common.h`**  
  Shared structure (`CanUdpFrame`) and constants (IP, MAC, UDP port). Ensures consistent frame format between CAN and UDP.

- **`Cpu0_Main.c`**  
  Main application code that runs on the AURIX TC375 Lite.  
  - **Board A:** Initializes CAN, listens for incoming CAN frames, and sends them via UDP.  
  - **Board B:** Initializes UDP receiver, listens for incoming UDP frames, and transmits them over CAN.  

*Test message ID `0x300h` is sent periodically with DLC=8 and data bytes `0x00`. An additional test frame `0x123h` with data `0xAA` is used for verification.*

---

## 🚀 Results

- **Logic Analyzer:** Confirmed periodic transmission of `0x300h` frames.  
- **PCAN-View:** Successfully received both `0x300h` (zero data) and `0x123h` (0xAA pattern) frames.  
- **Cycle Times:** 1 ms for test frame generation, stable reception over extended runtime.  

---

## 📚 References

( Infineon AURIX documentation, PEAK PCAN-USB manuals, lwIP reference)
