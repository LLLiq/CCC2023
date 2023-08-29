
#include <adf.h>
#include "kernels.h"
#include "system_settings.h"

using namespace adf;

class MyGraph : public adf::graph {
  private:


    kernel calc_step1_border_k[2];
    kernel calc_step2_border_k[2];


    kernel calc_step1_k[PART_NUMBER-2];
    kernel calc_step2_k[PART_NUMBER-2];


  public:
    input_plio in[PART_NUMBER];
    output_plio out[PART_NUMBER];

    input_plio coeff_in;

    MyGraph() {

      coeff_in = input_plio::create("CoeffData", plio_32_bits, "data/coeff.txt");

  	  in[0] = input_plio::create("StreamIn0", plio_32_bits, "data/input_p1.txt");
  	  in[1] = input_plio::create("StreamIn1", plio_32_bits, "data/input_p2.txt");
  	  in[2] = input_plio::create("StreamIn2", plio_32_bits, "data/input_p3.txt");
  	  in[3] = input_plio::create("StreamIn3", plio_32_bits, "data/input_p4.txt");
  	  in[4] = input_plio::create("StreamIn4", plio_32_bits, "data/input_p5.txt");
  	  in[5] = input_plio::create("StreamIn5", plio_32_bits, "data/input_p6.txt");
  	  in[6] = input_plio::create("StreamIn6", plio_32_bits, "data/input_p7.txt");
  	  in[7] = input_plio::create("StreamIn7", plio_32_bits, "data/input_p8.txt");

      out[0] = output_plio::create("StreamOut0", plio_32_bits, "data/output_p1.txt");
      out[1] = output_plio::create("StreamOut1", plio_32_bits, "data/output_p2.txt");
      out[2] = output_plio::create("StreamOut2", plio_32_bits, "data/output_p3.txt");
      out[3] = output_plio::create("StreamOut3", plio_32_bits, "data/output_p4.txt");
      out[4] = output_plio::create("StreamOut4", plio_32_bits, "data/output_p5.txt");
      out[5] = output_plio::create("StreamOut5", plio_32_bits, "data/output_p6.txt");
      out[6] = output_plio::create("StreamOut6", plio_32_bits, "data/output_p7.txt");
      out[7] = output_plio::create("StreamOut7", plio_32_bits, "data/output_p8.txt");

    int index = 0;


    calc_step1_border_k[0] = kernel::create(calc_step1_border);
    calc_step2_border_k[0] = kernel::create(calc_step2_border);

    connect< stream > c0( coeff_in.out[0],  calc_step1_border_k[0].in[1]);
    fifo_depth(c0) = 48;

	connect< stream > n0( in[0].out[0],  calc_step1_border_k[0].in[0]);
	fifo_depth(n0) = 512;
	connect< stream > n1(  calc_step1_border_k[0].out[0],  calc_step2_border_k[0].in[0]);
	connect< cascade > n2( calc_step1_border_k[0].out[1],  calc_step2_border_k[0].in[1]);
	fifo_depth(n1) = 64;
	connect< stream > n5( calc_step2_border_k[0].out[0], out[0].in[0]);

	source( calc_step1_border_k[0]) = "kernels/calc_step1_border.cpp";
	source( calc_step2_border_k[0]) = "kernels/calc_step2_border.cpp";

	runtime<ratio>(calc_step1_border_k[0]) = 1;
	runtime<ratio>(calc_step2_border_k[0]) = 1;

   // location<kernel>(calc_step1_border_k[0]) = adf::tile(index,0);
    //location<kernel>(calc_step2_border_k[0]) = adf::tile(index,1);
    index+=2;


    calc_step1_border_k[1] = kernel::create(calc_step1_border);
    calc_step2_border_k[1] = kernel::create(calc_step2_border);

    connect< stream > c1( coeff_in.out[0],  calc_step1_border_k[1].in[1]);
    fifo_depth(c1) = 48;

	connect< stream > n01( in[7].out[0],  calc_step1_border_k[1].in[0]);
	fifo_depth(n01) = 512;
	connect< stream > n11(  calc_step1_border_k[1].out[0],  calc_step2_border_k[1].in[0]);
	connect< cascade > n21( calc_step1_border_k[1].out[1],  calc_step2_border_k[1].in[1]);
	fifo_depth(n11) = 64;
	connect< stream > n51( calc_step2_border_k[1].out[0], out[7].in[0]);

	source( calc_step1_border_k[1]) = "kernels/calc_step1_border.cpp";
	source( calc_step2_border_k[1]) = "kernels/calc_step2_border.cpp";

	runtime<ratio>(calc_step1_border_k[1]) = 1;
	runtime<ratio>(calc_step2_border_k[1]) = 1;

    //location<kernel>(calc_step1_border_k[1]) = adf::tile(index,0);
    //location<kernel>(calc_step2_border_k[1]) = adf::tile(index,1);
    index+=2;


	for(int i=0;i<PART_NUMBER-2;i++){

		calc_step1_k[i] = kernel::create(calc_step1);
		calc_step2_k[i] = kernel::create(calc_step2);

	    connect< stream > c2( coeff_in.out[0],  calc_step1_k[i].in[1]);
	    fifo_depth(c2) = 48;

		connect< stream > nn0( in[i+1].out[0], calc_step1_k[i].in[0]);
		fifo_depth(nn0) = 512;

		connect< stream > nn1( calc_step1_k[i].out[0], calc_step2_k[i].in[0]);
		connect< cascade > nn2( calc_step1_k[i].out[1], calc_step2_k[i].in[1]);

		fifo_depth(nn1) = 64;

		connect< stream > nn5( calc_step2_k[i].out[0], out[i+1].in[0]);

		source(calc_step1_k[i]) = "kernels/calc_step1.cpp";
		source(calc_step2_k[i]) = "kernels/calc_step2.cpp";

		runtime<ratio>(calc_step2_k[i]) = 1;
		runtime<ratio>(calc_step1_k[i]) = 1;

	    //location<kernel>(calc_step1_k[i]) = adf::tile(index++,0);

	   // location<kernel>(calc_step2_k[i]) = adf::tile(index++,1);

	    index+=2;

	}



  };
  };
