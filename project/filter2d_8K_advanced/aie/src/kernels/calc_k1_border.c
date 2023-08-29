#include <adf.h>
#include "system_settings.h"

void calc_k1_border(input_stream_int32 * __restrict data_in, input_stream_acc80 * __restrict acc_in, output_stream_int32* __restrict data_out){


	v32int32 buf;
	v8acc80  acc;
	v8int32 out_buf;
	v8int32 kernel_v0,kernel_v1;

	kernel_v0 = upd_v(kernel_v0,0,readincr_v4(data_in));
	kernel_v0 = upd_v(kernel_v0,1,readincr_v4(data_in));
	kernel_v1 = upd_v(kernel_v1,0,readincr_v4(data_in));


	for(int n=0;n<PART_LINE_NUMBER-1;n++) chess_prepare_for_pipelining chess_loop_range(PART_LINE_NUMBER-1,PART_LINE_NUMBER-1){
		buf = upd_v(buf, 2, readincr_v4(data_in));
		buf = upd_v(buf, 3, readincr_v4(data_in));
		buf = upd_v(buf, 4, readincr_v4(data_in));
		buf = upd_v(buf, 5, readincr_v4(data_in));

		acc = upd_hi(acc,readincr_v4(acc_in));
		acc = upd_lo(acc,readincr_v4(acc_in));

		acc = lmac8(acc, buf, 8, 0x65432100, kernel_v0, 6, 0);      //  row2(00..06)*K6
		acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 7, 0); 		//  row2(00..07)*K7
		acc = lmac8(acc, buf, 9, 0x76543210, kernel_v1, 0, 0); 		//  row2(01..08)*K8

		out_buf = srs(acc,0);
		writeincr(data_out,ext_v(out_buf,0));
		writeincr(data_out,ext_v(out_buf,1));

		for(int i=2;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8-2,WIDTH/8-2){

			buf = upd_w(buf, 0, ext_w(buf,1));
			buf = upd_w(buf, 1, ext_w(buf,2));
			buf = upd_v(buf, 4, readincr_v4(data_in));
			buf = upd_v(buf, 5, readincr_v4(data_in));

			acc = upd_hi(acc,readincr_v4(acc_in));
			acc = upd_lo(acc,readincr_v4(acc_in));

			acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 7, 0);			//  row2(08..15)*K7
			acc = lmac8(acc, buf, 7, 0x76543210, kernel_v0, 6, 0); 		//  row2(07..14)*K6
			acc = lmac8(acc, buf, 9, 0x76543210, kernel_v1, 0, 0); 		//  row2(09..16)*K8

			out_buf = srs(acc,0);
			writeincr(data_out,ext_v(out_buf,0));
			writeincr(data_out,ext_v(out_buf,1));
		}

		acc = upd_hi(acc,readincr_v4(acc_in));
		acc = upd_lo(acc,readincr_v4(acc_in));

		acc = lmac8(acc, buf, 15, 0x76543210, kernel_v0, 6, 0); 		// kernel 6 (r2:07..14)
		acc = lmac8(acc, buf, 16, 0x76543210, kernel_v0, 7, 0); 		// kernel 7 (r2:08..15)
		acc = lmac8(acc, buf, 17, 0x66543210, kernel_v1, 0, 0); 		// kernel 8 (r2:09..15)

		out_buf = srs(acc,0);
		writeincr(data_out,ext_v(out_buf,0));
		writeincr(data_out,ext_v(out_buf,1));

	}


}
