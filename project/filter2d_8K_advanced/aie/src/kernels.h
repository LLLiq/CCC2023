
#ifndef KERNEL_H

#include <adf/window/types.h>
#include <adf/stream/types.h>


void distribute(input_stream_int32 * __restrict data_in,
				output_stream_int32* __restrict data_out_to_k0, output_stream_int32* __restrict data_out_to_k1);
void distribute_border(input_stream_int32 * __restrict data_in,
				output_stream_int32* __restrict data_out_to_k0, output_stream_int32* __restrict data_out_to_k1);

void calc_k0(input_stream_int32 * __restrict data_in, output_stream_acc80* __restrict acc_out);
void calc_k0_border(input_stream_int32 * __restrict data_in, output_stream_acc80* __restrict acc_out);

void calc_k1(input_stream_int32 * __restrict data_in, input_stream_acc80 * __restrict acc_in, output_stream_int32* __restrict data_out);
void calc_k1_border(input_stream_int32 * __restrict data_in, input_stream_acc80 * __restrict acc_in, output_stream_int32* __restrict data_out);


#endif
