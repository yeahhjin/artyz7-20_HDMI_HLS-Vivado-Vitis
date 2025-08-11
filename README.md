# artyz7-20_HDMI_HLS-Vivado-Vitis
HDMI Connectivity Project Using artyz7 20 board
---
🎯 Arty Z7-20 HDMI Sobel Filter Project (Vitis HLS + Vivado + Vitis)
📌 Project Overview
This project demonstrates the implementation of a custom video processing IP on FPGA using the Vitis HLS + Vivado + Vitis flow.
The intended target function is a Sobel edge detection filter, but for initial verification, the HLS IP is implemented as a pass-through module to ensure correct HDMI pipeline and AXI interface operation.

📝 Workflow
1. Vitis HLS
Designed a custom video processing IP (currently pass-through, future plan: Sobel filter) in C/C++

✅ Performed C Simulation (csim) and Co-Simulation (cosim)

✅ Exported RTL (Verilog)

2. Vivado
✅ Used Arty Z7-20 board files and Digilent’s HDMI example HW project as the base design

✅ Imported the HLS-generated IP into Vivado Block Design

✅ Connected AXI interfaces for control and video streaming

✅ Configured address mapping using Address Editor

3. Bitstream Generation
✅ Synthesized design and generated .bit file in Vivado

4. Vitis (SDK)
✅ Created an application project for the Zynq Processing System

✅ Implemented VDMA initialization and HDMI frame buffer management

✅ Executed the application to verify HDMI pass-through output

💻 Development Environment
Board: Digilent Arty Z7-20

FPGA Device: XC7Z020CLG400-1

Toolchain: Vitis HLS, Vivado, Vitis

Reference Files: Digilent HDMI In/Out example (HW & SW)

💡 Purpose
Build the base system for future Sobel Filter integration

Verify HDMI pipeline, AXI interface, and VDMA operation through pass-through test

Learn the complete FPGA design flow from C → RTL → Vivado → Vitis → HDMI Output

📊 Test Results
HDMI pass-through output successfully verified

Frame buffer and VDMA configured for continuous streaming without frame loss
