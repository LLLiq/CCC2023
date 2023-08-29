
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
  	  in[8] = input_plio::create("StreamIn8", plio_32_bits, "data/input_p9.txt");
      in[9] = input_plio::create("StreamIn9", plio_32_bits, "data/input_p10.txt");
      in[10] = input_plio::create("StreamIn10", plio_32_bits, "data/input_p11.txt");
      in[11] = input_plio::create("StreamIn11", plio_32_bits, "data/input_p12.txt");

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

	connect< stream > n01( in[11].out[0],  calc_step1_border_k[1].in[0]);
	fifo_depth(n01) = 512;
	connect< stream > n11(  calc_step1_border_k[1].out[0],  calc_step2_border_k[1].in[0]);
	connect< cascade > n21( calc_step1_border_k[1].out[1],  calc_step2_border_k[1].in[1]);
	fifo_depth(n11) = 64;
	connect< stream > n51( calc_step2_border_k[1].out[0], out[11].in[0]);

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

  	/*for(int i=0;i<PART_NUMBER-2;i++){

  		calc_core_k[i] = kernel::create(calc_core);

  		source(calc_core_k[i]) = "kernels/calc_core.cpp";

  		connect< stream > n0( in[i+1].out[0], calc_core_k[i].in[0]);
  		connect< stream > n2( calc_core_k[i].out[0], out[i+1].in[0]);

  		fifo_depth(n0) = 512;
  		fifo_depth(n2) = 512;
  		runtime<ratio>(calc_core_k[i]) = 1;
  	}*/


  };
  };

/*class MyGraph : public adf::graph {
  private:


    kernel calc0_k,calc1_k,calc2_k,calc3_k,calc4_k,calc5_k,calc6_k,calc7_k,calc8_k;
    kernel datawrite_k;

  public:
    input_plio in[9];
    output_plio out;

    MyGraph() {

	  in[0] = input_plio::create(plio_32_bits, "data/input_p1.txt");
	  in[1] = input_plio::create(plio_32_bits, "data/input_p1.txt");
	  in[2] = input_plio::create(plio_32_bits, "data/input_p1.txt");

	  in[3] = input_plio::create(plio_32_bits, "data/input_p2.txt");
	  in[4] = input_plio::create(plio_32_bits, "data/input_p2.txt");
	  in[5] = input_plio::create(plio_32_bits, "data/input_p2.txt");

	  in[6] = input_plio::create(plio_32_bits, "data/input_p3.txt");
	  in[7] = input_plio::create(plio_32_bits, "data/input_p3.txt");
	  in[8] = input_plio::create(plio_32_bits, "data/input_p3.txt");
      out = output_plio::create(plio_32_bits, "data/output.txt");

      calc0_k = kernel::create(calc0);
      calc1_k = kernel::create(calc1);
      calc2_k = kernel::create(calc2);
      calc3_k = kernel::create(calc3);
      calc4_k = kernel::create(calc4);
      calc5_k = kernel::create(calc5);
      calc6_k = kernel::create(calc6);
      calc7_k = kernel::create(calc7);
      calc8_k = kernel::create(calc8);
      datawrite_k = kernel::create(datawrite);


      connect< stream > n0( in[0].out[0], calc0_k.in[0]);
      connect< stream > n1( in[1].out[0], calc1_k.in[0]);
      connect< stream > n2( in[2].out[0], calc2_k.in[0]);
      connect< stream > n3( in[3].out[0], calc3_k.in[0]);
      connect< stream > n4( in[4].out[0], calc4_k.in[0]);
      connect< stream > n5( in[5].out[0], calc5_k.in[0]);
      connect< stream > n6( in[6].out[0], calc6_k.in[0]);
      connect< stream > n7( in[7].out[0], calc7_k.in[0]);
      connect< stream > n8( in[8].out[0], calc8_k.in[0]);

      connect< cascade > n09( calc0_k.out[0], calc1_k.in[1]);
      connect< cascade > n10( calc1_k.out[0], calc2_k.in[1]);
      connect< cascade > n11( calc2_k.out[0], calc3_k.in[1]);
      connect< cascade > n12( calc3_k.out[0], calc4_k.in[1]);
      connect< cascade > n13( calc4_k.out[0], calc5_k.in[1]);
      connect< cascade > n14( calc5_k.out[0], calc6_k.in[1]);
      connect< cascade > n15( calc6_k.out[0], calc7_k.in[1]);
      connect< cascade > n16( calc7_k.out[0], calc8_k.in[1]);

      connect< cascade > n17( calc8_k.out[0], datawrite_k.in[0]);
      connect< stream > n18( datawrite_k.out[0], out.in[0]);

     // source(datagen_k) =  "kernels/datagen.c";
      source(calc0_k) = "kernels/calc0.cpp";
      source(calc1_k) = "kernels/calc1.cpp";
      source(calc2_k) = "kernels/calc2.cpp";
      source(calc3_k) = "kernels/calc3.cpp";
      source(calc4_k) = "kernels/calc4.cpp";
      source(calc5_k) = "kernels/calc5.cpp";
      source(calc6_k) = "kernels/calc6.cpp";
      source(calc7_k) = "kernels/calc7.cpp";
      source(calc8_k) = "kernels/calc8.cpp";
      source(datawrite_k) = "kernels/datawrite.cpp";

     //runtime<ratio>(datagen_k) = 1;
      runtime<ratio>(calc0_k) = 1;
      runtime<ratio>(calc1_k) = 1;
      runtime<ratio>(calc2_k) = 1;
      runtime<ratio>(calc3_k) = 1;
      runtime<ratio>(calc4_k) = 1;
      runtime<ratio>(calc5_k) = 1;
      runtime<ratio>(calc6_k) = 1;
      runtime<ratio>(calc7_k) = 1;
      runtime<ratio>(calc8_k) = 1;
      runtime<ratio>(datawrite_k) = 1;

      fifo_depth(n0) = 2048;
      fifo_depth(n1) = 2048;
      fifo_depth(n2) = 2048;
      fifo_depth(n3) = 2048;
	  fifo_depth(n4) = 2048;
	  fifo_depth(n5) = 2048;
	  fifo_depth(n6) = 2048;
	  fifo_depth(n7) = 2048;
	  fifo_depth(n8) = 2048;

  };
  };*/


