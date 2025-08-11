############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project sobel_0805
set_top sobel_filter
add_files sobel_0805/sobel_filter.cpp
add_files sobel_0805/sobel_filter.h
add_files -tb sobel_0805/sobel_test.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_cosim -tool xsim
config_export -format ip_catalog -rtl verilog
source "./sobel_0805/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
