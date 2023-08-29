
#ifndef KERNEL_H

#include <adf/window/types.h>
#include <adf/stream/types.h>


	void calc_step2(input_stream_int32 * __restrict data_in, input_stream_acc80* __restrict acc_in,output_stream_int32* __restrict data_out);
	void calc_step2_border(input_stream_int32 * __restrict data_in, input_stream_acc80* __restrict acc_in,output_stream_int32* __restrict data_out);

	void calc_step1_border(input_stream_int32 * __restrict data_in, input_stream_int32 * __restrict coeff_in,
			output_stream_int32* __restrict data_out, output_stream_acc80* __restrict acc_out);
	void calc_step1(input_stream_int32 * __restrict data_in, input_stream_int32 * __restrict coeff_in,
			output_stream_int32* __restrict data_out, output_stream_acc80* __restrict acc_out);


#endif
