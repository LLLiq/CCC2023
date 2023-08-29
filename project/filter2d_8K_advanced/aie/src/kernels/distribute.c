#include <adf.h>
#include "system_settings.h"

void distribute(input_stream_int32 * __restrict data_in,
				output_stream_int32* __restrict data_out_to_k0, output_stream_int32* __restrict data_out_to_k1){

	for(int i=0;i<3;i++)
	chess_prepare_for_pipelining
	chess_loop_range(3,3){
		v4int32 kernel_buf;
		kernel_buf = readincr_v4(data_in);
		writeincr_v4(data_out_to_k0,kernel_buf);
		writeincr_v4(data_out_to_k1,kernel_buf);
	}

	v4int32 line_buf[WIDTH/4];

	for(int i=0;i<WIDTH/4;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/4,WIDTH/4){
		writeincr_v4(data_out_to_k0,readincr_v4(data_in));
	}

	for(int i=0;i<WIDTH/4;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/4,WIDTH/4){
		line_buf[i] = readincr_v4(data_in);
	}

	for(int n=0;n<PART_LINE_NUMBER;n++) chess_prepare_for_pipelining chess_loop_range(PART_LINE_NUMBER,PART_LINE_NUMBER){

		for(int i=0;i<WIDTH/4;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/4,WIDTH/4){

			writeincr_v4(data_out_to_k0,line_buf[i]);

			line_buf[i] = readincr_v4(data_in);

			writeincr_v4(data_out_to_k1,line_buf[i]);

		}

	}

}
