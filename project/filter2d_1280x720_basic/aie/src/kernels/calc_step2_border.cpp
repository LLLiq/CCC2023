#include <adf.h>
#include "system_settings.h"

void calc_step2_border(input_stream_int32 * __restrict data_in, input_stream_acc80* __restrict acc_in,output_stream_int32* __restrict data_out){

	v8int32 kernel_v0,kernel_v1;
    v32int32 buf,buf1;
	v8acc80 acc;
	v8int32 out_buf;
	v8int32 line_buf[WIDTH/8];//图像buffer

	kernel_v0 = upd_v(kernel_v0,0,readincr_v4(data_in));
	kernel_v0 = upd_v(kernel_v0,1,readincr_v4(data_in));
	kernel_v1 = upd_v(kernel_v1,0,readincr_v4(data_in));

	//读入第2行图像
	for(int i=0;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8,WIDTH/8){
		line_buf[i] = upd_v(line_buf[i],0,readincr_v4(data_in));
		line_buf[i] = upd_v(line_buf[i],1,readincr_v4(data_in));
	}

	for(int n=0;n<PART_LINE_NUMBER-1;n++)chess_prepare_for_pipelining chess_loop_range(PART_LINE_NUMBER-1,PART_LINE_NUMBER-1){


		//准备运算kernel[3~5]所需数据
		buf = upd_w(buf, 1, line_buf[0]);
		buf = upd_w(buf, 2, line_buf[1]);
		//读入ACC
		acc = upd_hi(acc,readincr_v4(acc_in));
		acc = upd_lo(acc,readincr_v4(acc_in));
		//运算kernel[3~5]
		acc = lmac8(acc, buf, 8, 0x65432100, kernel_v0, 3, 0);     	//  row1(00..06)*K3
		acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 4, 0); 		//  row1(00..07)*K4
		acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 5, 0); 		//  row1(01..08)*K5

		//读入数据并更新line_buf
		line_buf[0] = upd_v(line_buf[0], 0, readincr_v4(data_in));
		line_buf[0] = upd_v(line_buf[0], 1, readincr_v4(data_in));
		line_buf[1] = upd_v(line_buf[1], 0, readincr_v4(data_in));
		line_buf[1] = upd_v(line_buf[1], 1, readincr_v4(data_in));

		//准备数据并运算kernel[6~8]
		buf1 = upd_w(buf1, 1, line_buf[0]);
		buf1 = upd_w(buf1, 2, line_buf[1]);
		acc = lmac8(acc, buf1, 8, 0x65432100, kernel_v0, 6, 0);     //  row2(00..06)*K6
		acc = lmac8(acc, buf1, 8, 0x76543210, kernel_v0, 7, 0); 	//  row2(00..07)*K7
		acc = lmac8(acc, buf1, 9, 0x76543210, kernel_v1, 0, 0); 	//  row2(01..08)*K8

		//写出结果
		out_buf = srs(acc,0);
		writeincr(data_out,ext_elem(out_buf,1));
		writeincr(data_out,ext_elem(out_buf,2));
		writeincr(data_out,ext_elem(out_buf,3));
		writeincr(data_out,ext_v(out_buf,1));

		for(int i=2;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8-2,WIDTH/8-2){

			//准备运算kernel[3~5]所需数据
			buf = upd_w(buf, 0, ext_w(buf,1));
			buf = upd_w(buf, 1, ext_w(buf,2));
			buf = upd_w(buf, 2, line_buf[i]);
			//读入ACC
			acc = upd_hi(acc,readincr_v4(acc_in));
			acc = upd_lo(acc,readincr_v4(acc_in));
			//运算kernel[3~5]
			acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 4, 0); 			//  row1(08..15)*K4
			acc = lmac8(acc, buf, 7, 0x76543210, kernel_v0, 3, 0); 			//  row1(07..14)*K3
			acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 5, 0); 			//  row1(09..16)*K5

			//读入数据并更新line_buf
			line_buf[i] = upd_v(line_buf[i], 0, readincr_v4(data_in));
			line_buf[i] = upd_v(line_buf[i], 1, readincr_v4(data_in));

			//准备数据并运算kernel[6~8]
			buf1 = upd_w(buf1, 0, ext_w(buf1,1));
			buf1 = upd_w(buf1, 1, ext_w(buf1,2));
			buf1 = upd_w(buf1, 2, line_buf[i]);
			acc = lmac8(acc, buf1, 8, 0x76543210, kernel_v0, 7, 0);			//  row2(08..15)*K7
			acc = lmac8(acc, buf1, 7, 0x76543210, kernel_v0, 6, 0); 		//  row2(07..14)*K6
			acc = lmac8(acc, buf1, 9, 0x76543210, kernel_v1, 0, 0); 		//  row2(09..16)*K8

			//写出结果
			out_buf = srs(acc,0);
			writeincr(data_out,ext_v(out_buf,0));
			writeincr(data_out,ext_v(out_buf,1));
		}

		acc = upd_hi(acc,readincr_v4(acc_in));
		acc = upd_lo(acc,readincr_v4(acc_in));

		acc = lmac8(acc, buf, 15, 0x76543210, kernel_v0, 3, 0);	 		// kernel 3 (r1:07..14)
		acc = lmac8(acc, buf, 16, 0x76543210, kernel_v0, 4, 0); 		// kernel 4 (r1:08..15)
		acc = lmac8(acc, buf, 17, 0x66543210, kernel_v0, 5, 0); 		// kernel 5 (r1:09..15)

		acc = lmac8(acc, buf1, 15, 0x76543210, kernel_v0, 6, 0); 		// kernel 6 (r2:07..14)
		acc = lmac8(acc, buf1, 16, 0x76543210, kernel_v0, 7, 0); 		// kernel 7 (r2:08..15)
		acc = lmac8(acc, buf1, 17, 0x66543210, kernel_v1, 0, 0); 		// kernel 8 (r2:09..15)

		out_buf = srs(acc,0);
		writeincr(data_out,ext_v(out_buf,0));
		writeincr(data_out,ext_elem(out_buf,4));
		writeincr(data_out,ext_elem(out_buf,5));
		writeincr(data_out,ext_elem(out_buf,6));

	}


}
