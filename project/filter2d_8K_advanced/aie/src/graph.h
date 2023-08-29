
#include <adf.h>
#include "kernels.h"
#include "system_settings.h"

using namespace adf;

class MyGraph : public adf::graph {
  private:


    kernel distribute_border_firstK,calc_k0_border_firstK,calc_k1_border_firstK;

    kernel distribute_K[PART_NUMBER-2],calc_k0_K[PART_NUMBER-2],calc_k1_K[PART_NUMBER-2];

    kernel distribute_border_lastK,calc_k0_border_lastK,calc_k1_border_lastK;


  public:
    input_plio in[PART_NUMBER];
    output_plio out[PART_NUMBER];

    MyGraph() {

  	  in[0] = input_plio::create("StreamIn0", plio_32_bits, "data/input_p1.txt");
  	  in[1] = input_plio::create("StreamIn1", plio_32_bits, "data/input_p2.txt");
  	  in[2] = input_plio::create("StreamIn2", plio_32_bits, "data/input_p3.txt");
  	  in[3] = input_plio::create("StreamIn3", plio_32_bits, "data/input_p4.txt");
  	  in[4] = input_plio::create("StreamIn4", plio_32_bits, "data/input_p5.txt");
  	  in[5] = input_plio::create("StreamIn5", plio_32_bits, "data/input_p6.txt");
  	  in[6] = input_plio::create("StreamIn6", plio_32_bits, "data/input_p7.txt");
  	  in[7] = input_plio::create("StreamIn7", plio_32_bits, "data/input_p8.txt");
  	  in[8] = input_plio::create("StreamIn8", plio_32_bits, "data/input_p9.txt");
      in[9] = input_plio::create("StreamIn9", plio_32_bits, "data/input_p10.txt");
      in[10] = input_plio::create("StreamIn10", plio_32_bits, "data/input_p11.txt");
      in[11] = input_plio::create("StreamIn11", plio_32_bits, "data/input_p12.txt");
  	  in[12] = input_plio::create("StreamIn12", plio_32_bits, "data/input_p13.txt");
      in[13] = input_plio::create("StreamIn13", plio_32_bits, "data/input_p14.txt");
      in[14] = input_plio::create("StreamIn14", plio_32_bits, "data/input_p15.txt");
      in[15] = input_plio::create("StreamIn15", plio_32_bits, "data/input_p16.txt");
	  in[16] = input_plio::create("StreamIn16", plio_32_bits, "data/input_p17.txt");
      in[17] = input_plio::create("StreamIn17", plio_32_bits, "data/input_p18.txt");

      out[0] = output_plio::create("StreamOut0", plio_32_bits, "data/output_p1.txt");
      out[1] = output_plio::create("StreamOut1", plio_32_bits, "data/output_p2.txt");
      out[2] = output_plio::create("StreamOut2", plio_32_bits, "data/output_p3.txt");
      out[3] = output_plio::create("StreamOut3", plio_32_bits, "data/output_p4.txt");
      out[4] = output_plio::create("StreamOut4", plio_32_bits, "data/output_p5.txt");
      out[5] = output_plio::create("StreamOut5", plio_32_bits, "data/output_p6.txt");
      out[6] = output_plio::create("StreamOut6", plio_32_bits, "data/output_p7.txt");
      out[7] = output_plio::create("StreamOut7", plio_32_bits, "data/output_p8.txt");
      out[8] = output_plio::create("StreamOut8", plio_32_bits, "data/output_p9.txt");
      out[9] = output_plio::create("StreamOut9", plio_32_bits, "data/output_p10.txt");
      out[10] = output_plio::create("StreamOut10", plio_32_bits, "data/output_p11.txt");
      out[11] = output_plio::create("StreamOut11", plio_32_bits, "data/output_p12.txt");
      out[12] = output_plio::create("StreamOut12", plio_32_bits, "data/output_p13.txt");
      out[13] = output_plio::create("StreamOut13", plio_32_bits, "data/output_p14.txt");
      out[14] = output_plio::create("StreamOut14", plio_32_bits, "data/output_p15.txt");
      out[15] = output_plio::create("StreamOut15", plio_32_bits, "data/output_p16.txt");
	  out[16] = output_plio::create("StreamOut16", plio_32_bits, "data/output_p17.txt");
      out[17] = output_plio::create("StreamOut17", plio_32_bits, "data/output_p18.txt");


      distribute_border_firstK = kernel::create(distribute_border);
      calc_k0_border_firstK = kernel::create(calc_k0_border);
      calc_k1_border_firstK = kernel::create(calc_k1_border);

	  connect< stream > d0( in[0].out[0],  distribute_border_firstK.in[0]);fifo_depth(d0) = 512;
	  //connect< stream > k0( coeff_in.out[0],  distribute_border_firstK.in[1]);

	  connect< stream > d1( distribute_border_firstK.out[0], calc_k0_border_firstK.in[0]);fifo_depth(d1) = 512;
	  connect< stream > d2( distribute_border_firstK.out[1], calc_k1_border_firstK.in[0]);fifo_depth(d2) = 512;

	  connect< cascade> c1( calc_k0_border_firstK.out[0],calc_k1_border_firstK.in[1]);

	  connect< stream > d3( calc_k1_border_firstK.out[0], out[0].in[0]);fifo_depth(d3) = 512;

	  source( distribute_border_firstK ) = "kernels/distribute_border.c";
	  source( calc_k0_border_firstK ) = "kernels/calc_k0_border.c";
	  source( calc_k1_border_firstK ) = "kernels/calc_k1_border.c";

	  runtime<ratio>(distribute_border_firstK) = 1;
	  runtime<ratio>(calc_k0_border_firstK) = 1;
	  runtime<ratio>(calc_k1_border_firstK) = 1;



	  distribute_border_lastK = kernel::create(distribute_border);
	  calc_k0_border_lastK = kernel::create(calc_k0_border);
	  calc_k1_border_lastK = kernel::create(calc_k1_border);

	  connect< stream > d01( in[PART_NUMBER-1].out[0],  distribute_border_lastK.in[0]);fifo_depth(d01) = 512;
	  //connect< stream > k01( coeff_in.out[0],  distribute_border_lastK.in[1]);

	  connect< stream > d11( distribute_border_lastK.out[0], calc_k0_border_lastK.in[0]);fifo_depth(d11) = 512;
	  connect< stream > d21( distribute_border_lastK.out[1], calc_k1_border_lastK.in[0]);fifo_depth(d21) = 512;

	  connect< cascade> c11( calc_k0_border_lastK.out[0],calc_k1_border_lastK.in[1]);

	  connect< stream > d31( calc_k1_border_lastK.out[0], out[PART_NUMBER-1].in[0]);fifo_depth(d31) = 512;

	  source( distribute_border_lastK ) = "kernels/distribute_border.c";
	  source( calc_k0_border_lastK ) = "kernels/calc_k0_border.c";
	  source( calc_k1_border_lastK ) = "kernels/calc_k1_border.c";

	  runtime<ratio>(distribute_border_lastK) = 1;
	  runtime<ratio>(calc_k0_border_lastK) = 1;
	  runtime<ratio>(calc_k1_border_lastK) = 1;


	for(int i=0;i<PART_NUMBER-2;i++){

		  distribute_K[i] = kernel::create(distribute);
	      calc_k0_K[i] = kernel::create(calc_k0);
	      calc_k1_K[i] = kernel::create(calc_k1);

		  connect< stream > d0( in[i+1].out[0],  distribute_K[i].in[0]);fifo_depth(d0) = 512;
		  //connect< stream > k0( coeff_in.out[0],  distribute_K[i].in[1]);

		  connect< stream > d1( distribute_K[i].out[0], calc_k0_K[i].in[0]);fifo_depth(d1) = 512;
		  connect< stream > d2( distribute_K[i].out[1], calc_k1_K[i].in[0]);fifo_depth(d2) = 512;

		  connect< cascade> c1( calc_k0_K[i].out[0],calc_k1_K[i].in[1]);

		  connect< stream > d3( calc_k1_K[i].out[0], out[i+1].in[0]);fifo_depth(d3) = 512;

		  source( distribute_K[i] ) = "kernels/distribute.c";
		  source( calc_k0_K[i] ) = "kernels/calc_k0.c";
		  source( calc_k1_K[i] ) = "kernels/calc_k1.c";

		  runtime<ratio>(distribute_K[i]) = 1;
		  runtime<ratio>(calc_k0_K[i]) = 1;
		  runtime<ratio>(calc_k1_K[i]) = 1;

	}



  };
  };



