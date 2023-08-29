#ifndef __PL_SETTINGS_H__
#define __PL_SETTINGS_H__

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <cstring>
#include <string>

#define WIDTH 7680
#define HEIGHT 4320
#define PART_NUMBER 18
#define PART_LINE_NUMBER HEIGHT/PART_NUMBER

#define RECEIVE_PART_NUMBER 2

typedef hls::stream<qdma_axis<32, 0, 0, 0>> stream_t;
typedef ap_int<32> data_t;

#endif
