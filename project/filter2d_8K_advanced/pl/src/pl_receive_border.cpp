#include "pl_settings.h"

void pl_receive_border(ap_int<32>* mem,stream_t& in0,  stream_t& in1){

	data_t buf[16];
	#pragma HLS ARRAY_PARTITION variable=buf dim=1 complete


	for(int i=0;i<(PART_LINE_NUMBER-1) *2;i++){
		for(int j=0;j<WIDTH/16;j++){
			for(int n=0;n<8;n++){
				buf[0*8 + n] = in0.read().data;
				buf[1*8 + n] = in1.read().data;
			}
			memcpy(mem + i*WIDTH + 16*j ,buf,sizeof(data_t)*16);
		}
	}
}
