#include <adf.h>
#include "system_settings.h"

void calc_step1(input_stream_int32 * __restrict data_in, input_stream_int32 * __restrict coeff_in,
		output_stream_int32* __restrict data_out, output_stream_acc80* __restrict acc_out){

	v4int32 kernel_buf[3];//kernel buffer
    v8int32 kernel_v0;
	v8acc80 acc;
	v32int32 buf;//运算buffer
	v8int32 line_buf[2][WIDTH/8];//图像buffer

	//读入并写出kernel
	for(int i=0;i<3;i++)
	chess_prepare_for_pipelining
	chess_loop_range(3,3){
		kernel_buf[i] = readincr_v4(coeff_in);
		writeincr_v4(data_out,kernel_buf[i]);
	}
	kernel_v0 = upd_v(kernel_v0,0,kernel_buf[0]);
	kernel_v0 = upd_v(kernel_v0,1,kernel_buf[1]);

	//预读入1行图像
	for(int i=0;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8,WIDTH/8){
		line_buf[1][i] = upd_v(line_buf[1][i],0,readincr_v4(data_in));
		line_buf[1][i] = upd_v(line_buf[1][i],1,readincr_v4(data_in));
	}
	//预读入1行图像 + 写出1行
	for(int i=0;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8,WIDTH/8){
		line_buf[0][i] = upd_v(line_buf[0][i],0,readincr_v4(data_in));
		line_buf[0][i] = upd_v(line_buf[0][i],1,readincr_v4(data_in));
		writeincr_v4(data_out,ext_v(line_buf[0][i],0));
		writeincr_v4(data_out,ext_v(line_buf[0][i],1));
	}

	//|0 1 2 3 4 5 6 7 | 8 9 10 11 12 13 14 15 | 16 17 18 19 20 21 22 23 |
	//|  0    | 1      |  2       |  3         |  4         |  5         |
	//|  0             |  1                    |  2                      |
	for(int n=0;n<PART_LINE_NUMBER;n++)chess_prepare_for_pipelining chess_loop_range(PART_LINE_NUMBER,PART_LINE_NUMBER){
		//处理第一块v8int32,只运算kernel[0~2]
		buf = upd_w(buf, 1, line_buf[1][0]);
		buf = upd_w(buf, 2, line_buf[1][1]);
		acc = lmul8(buf, 8, 0x65432100, kernel_v0, 0, 0);      			//  row0(00..06)*K0
		acc = lmac8(acc, buf, 8, 0x76543210, kernel_v0, 1, 0); 			//  row0(00..07)*K1
		acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 2, 0); 			//  row0(01..08)*K2

		//写出ACC
		writeincr_v4(acc_out,ext_hi(acc));
		writeincr_v4(acc_out,ext_lo(acc));
		//上移
		line_buf[1][0] = line_buf[0][0];
		line_buf[1][1] = line_buf[0][1];
		//读取新数据
		line_buf[0][0] = upd_v(line_buf[0][0],0,readincr_v4(data_in));
		line_buf[0][0] = upd_v(line_buf[0][0],1,readincr_v4(data_in));
		line_buf[0][1] = upd_v(line_buf[0][1],0,readincr_v4(data_in));
		line_buf[0][1] = upd_v(line_buf[0][1],1,readincr_v4(data_in));
		//写出数据
		writeincr_v4(data_out,ext_v(line_buf[0][0],0));
		writeincr_v4(data_out,ext_v(line_buf[0][0],1));
		writeincr_v4(data_out,ext_v(line_buf[0][1],0));
		writeincr_v4(data_out,ext_v(line_buf[0][1],1));


		for(int i=2;i<WIDTH/8;i++) chess_prepare_for_pipelining chess_loop_range(WIDTH/8-2,WIDTH/8-2){
			//处理中间块,只运算kernel[0~2]
			buf = upd_w(buf, 0, ext_w(buf,1));
			buf = upd_w(buf, 1, ext_w(buf,2));
			buf = upd_w(buf, 2, line_buf[1][i]);
			acc = lmul8(buf, 8, 0x76543210, kernel_v0, 1, 0); 				//  row0(08..15)*K1
			acc = lmac8(acc, buf, 7, 0x76543210, kernel_v0, 0, 0); 			//  row0(07..14)*K0
			acc = lmac8(acc, buf, 9, 0x76543210, kernel_v0, 2, 0); 			//  row0(09..16)*K2

			//写出ACC
			writeincr_v4(acc_out,ext_hi(acc));
			writeincr_v4(acc_out,ext_lo(acc));
			//上移
			line_buf[1][i] = line_buf[0][i];
			//读取新数据
			line_buf[0][i] = upd_v(line_buf[0][i],0,readincr_v4(data_in));
			line_buf[0][i] = upd_v(line_buf[0][i],1,readincr_v4(data_in));
			//写出数据
			writeincr_v4(data_out,ext_v(line_buf[0][i],0));
			writeincr_v4(data_out,ext_v(line_buf[0][i],1));

		}
		//处理最后一块,只运算kernel[0~2]
		acc = lmul8(buf, 15, 0x76543210, kernel_v0, 0, 0);     		// kernel 0 (r1:07..14)
		acc = lmac8(acc, buf, 16, 0x76543210, kernel_v0, 1, 0); 		// kernel 1 (r1:08..15)
		acc = lmac8(acc, buf, 17, 0x66543210, kernel_v0, 2, 0); 		// kernel 2 (r1:09..15)
		//写出ACC
		writeincr_v4(acc_out,ext_hi(acc));
		writeincr_v4(acc_out,ext_lo(acc));
	}


}
