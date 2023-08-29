#include <adf.h>
#include "system_settings.h"

void calc_k0_border(input_stream_int32 * __restrict data_in, output_stream_acc80* __restrict acc_out){

	v32int32 buf,buf1;
	v8acc80  acc;
	v8int32 line_buf[WIDTH/8];

	v8int32 kernel_v0,kernel_v1;
	kernel_v0 = upd_v(kernel_v0,0,readincr_v4(data_in));
	kernel_v0 = upd_v(kernel_v0,1,readincr_v4(data_in));
	kernel_v1 = upd_v(kernel_v1,0,readincr_v4(data_in));

	//读进来一行
	for(int i=0;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8,WIDTH/8){
		line_buf[i] = upd_v(line_buf[i],0,readincr_v4(data_in));
		line_buf[i] = upd_v(line_buf[i],1,readincr_v4(data_in));
	}

	//|0 1 2 3 4 5 6 7 | 8 9 10 11 12 13 14 15 | 16 17 18 19 20 21 22 23 |
	//|  0    | 1      |  2       |  3         |  4         |  5         |
	//|  0             |  1                    |  2                      |
	for(int n=0;n<PART_LINE_NUMBER-1;n++) chess_prepare_for_pipelining chess_loop_range(PART_LINE_NUMBER-1,PART_LINE_NUMBER-1){

		buf = upd_w(buf, 1, line_buf[0]);
		buf = upd_w(buf, 2, line_buf[1]);
		acc = lmul8(buf, 8, 0x65432100, kernel_v0, 0, 0);      			//  row0(00..06)*K0
		acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 1, 0); 			//  row0(00..07)*K1
		acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 2, 0); 			//  row0(01..08)*K2

		line_buf[0] = upd_v(line_buf[0],0,readincr_v4(data_in));
		line_buf[0] = upd_v(line_buf[0],1,readincr_v4(data_in));
		line_buf[1] = upd_v(line_buf[1],0,readincr_v4(data_in));
		line_buf[1] = upd_v(line_buf[1],1,readincr_v4(data_in));

		buf1 = upd_w(buf1, 1, line_buf[0]);
		buf1 = upd_w(buf1, 2, line_buf[1]);
		acc = lmac8(acc, buf1, 8, 0x65432100, kernel_v0, 3, 0);     	//  row1(00..06)*K3
		acc = lmac8(acc, buf1, 8, 0x76543210, kernel_v0, 4, 0); 		//  row1(00..07)*K4
		acc = lmac8(acc, buf1, 9, 0x76543210, kernel_v0, 5, 0); 		//  row1(01..08)*K5

		writeincr_v4(acc_out,ext_hi(acc));
		writeincr_v4(acc_out,ext_lo(acc));

		for(int i=2;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8-2,WIDTH/8-2){

			buf = upd_w(buf, 0, ext_w(buf,1));
			buf = upd_w(buf, 1, ext_w(buf,2));
			buf = upd_w(buf, 2, line_buf[i]);
			acc = lmul8(buf, 8, 0x76543210, kernel_v0, 1, 0); 				//  row0(08..15)*K1
			acc = lmac8(acc, buf, 7, 0x76543210, kernel_v0, 0, 0); 			//  row0(07..14)*K0
			acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 2, 0); 			//  row0(09..16)*K2

			line_buf[i] = upd_v(line_buf[i],0,readincr_v4(data_in));
			line_buf[i] = upd_v(line_buf[i],1,readincr_v4(data_in));


			buf1 = upd_w(buf1, 0, ext_w(buf1,1));
			buf1 = upd_w(buf1, 1, ext_w(buf1,2));
			buf1 = upd_w(buf1, 2, line_buf[i]);
			acc = lmac8(acc, buf1, 8, 0x76543210, kernel_v0, 4, 0); 			//  row1(08..15)*K4
			acc = lmac8(acc, buf1, 7, 0x76543210, kernel_v0, 3, 0); 			//  row1(07..14)*K3
			acc = lmac8(acc, buf1, 9, 0x76543210, kernel_v0, 5, 0); 			//  row1(09..16)*K5

			writeincr_v4(acc_out,ext_hi(acc));
			writeincr_v4(acc_out,ext_lo(acc));
		}

		acc = lmul8(buf, 15, 0x76543210, kernel_v0, 0, 0);     			// kernel 0 (r1:07..14)
		acc = lmac8(acc, buf, 16, 0x76543210, kernel_v0, 1, 0); 		// kernel 1 (r1:08..15)
		acc = lmac8(acc, buf, 17, 0x66543210, kernel_v0, 2, 0); 		// kernel 2 (r1:09..15)

		acc = lmac8(acc, buf1, 15, 0x76543210, kernel_v0, 3, 0);	 		// kernel 3 (r1:07..14)
		acc = lmac8(acc, buf1, 16, 0x76543210, kernel_v0, 4, 0); 		// kernel 4 (r1:08..15)
		acc = lmac8(acc, buf1, 17, 0x66543210, kernel_v0, 5, 0); 		// kernel 5 (r1:09..15)

		writeincr_v4(acc_out,ext_hi(acc));
		writeincr_v4(acc_out,ext_lo(acc));

	}


}
