#pragma once
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

// 24-bit RGB AXIS (Video side-channels Æ÷ÇÔ)
typedef ap_axiu<24, 1, 1, 1> pixel_t;
typedef hls::stream<pixel_t>  AXI_STREAM;

// Top (ap_ctrl_none)
void sobel_filter(AXI_STREAM &input, AXI_STREAM &output);
