#include "pl_settings.h"

void pl_send_border(ap_int<32>* mem, stream_t& out){

	//send coeff before send image data
	for(int i=0;i<12;i++){
		qdma_axis<32, 0, 0, 0> x;
        x.data = mem[i];
        out.write(x);
	}
	mem+=12;

	data_t buf[16];
	#pragma HLS ARRAY_PARTITION variable=buf dim=1 complete


	for(int i=0;i<PART_LINE_NUMBER+1;i++){

		for(int j=0;j<WIDTH/16;j++){

			memcpy(buf, mem + i*WIDTH + 16*j,sizeof(data_t)*16);

			for(int n=0;n<16;n++){
				qdma_axis<32, 0, 0, 0> tmp;
				tmp.data = buf[n];
				out.write(tmp);
			}

		}

	}
}
