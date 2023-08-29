#include <cstring>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iomanip>
#include <chrono>
#include <ratio>


#include <xrt/xrt_bo.h>
#include <xrt/xrt_kernel.h>
#include "experimental/xrt_kernel.h"

//filter2D algorithm on PS
int* filter2D(int* img, unsigned int img_size, const int* kernel, unsigned int kernel_size) {
	//compute the result size
	unsigned int result_size = img_size - kernel_size + 1;
	//Allocate memory for result
	int* result = new int[result_size * result_size];
	memset(result, 0, sizeof(int) * result_size * result_size);
	//compute the result
	for (unsigned int row = 0; row < result_size; ++row) {
		for (unsigned int col = 0; col < result_size; ++col) {
			unsigned int iter_kernel = 0;
			//compute the result[row][column]
			for (unsigned int iter_r = 0; iter_r < kernel_size; ++iter_r) {
				for (unsigned int iter_c = 0; iter_c < kernel_size; ++iter_c) {
					result[row * result_size + col] += img[(row + iter_r) * img_size + col + iter_c] * kernel[iter_kernel++];
				}
			}
		}
	}
	return result;
}


int main(int argc,char** argv){
	//the constant of filter2D program
	const int img_size=64;
	const int conv_kernal_size=3;
	const int pl_kernel_num=8;

	//filter2D kernel
	// const int conv_kernel[16] = {1, 2, 3, 1, 0, 0, 2, 1, 5,0,0,0,0,0,0,0};
	const int conv_kernel[16] = {10, 7, 4,
								 5, 6, 9,
								 2, 3, 3,
								 0,0,0};

	//Allocate memory for result
	unsigned int result_size=img_size-conv_kernal_size+1;
	//AIE filter2D result
	int aie_result[result_size*result_size];
	memset(aie_result,0,sizeof(int)*result_size*result_size);

	//Get device index and load xclbin
	std::cout<<"open the device"<<std::endl;
	auto device=xrt::device(0);
	std::string binaryFile = argv[1];
	std::cout<<"Load the xclbin "<<binaryFile<<std::endl;
	auto uuid = device.load_xclbin(binaryFile);
	

	// Get reference to the PL kernels
	std::cout<<"Get Kernel Reference"<<std::endl;
	xrt::kernel pl_mm2s[pl_kernel_num],pl_s2mm[pl_kernel_num];
	for(int i=0;i<pl_kernel_num;++i){
		std::string kernel_name="pl_mm2s:{pl_mm2s_"+std::to_string(i+1)+'}';
		pl_mm2s[i]=xrt::kernel(device,uuid,kernel_name);

		kernel_name="pl_s2mm:{pl_s2mm_"+std::to_string(i+1)+'}';
		pl_s2mm[i]=xrt::kernel(device,uuid,kernel_name);
	}

	std::string kernel_name="pl_mm2s:{pl_mm2s_"+std::to_string(pl_kernel_num+1)+'}';
	auto conv_kernal_mm2s=xrt::kernel(device,uuid,kernel_name);


	//initial the image
	std::cout<<"Initial the image"<<std::endl;
	int img[img_size*img_size];
	for(int i=0;i<img_size*img_size;++i) img[i]=i;

	//compute the input and output data length
	int inputDataLength[pl_kernel_num],outputDataLength[pl_kernel_num];
	inputDataLength[0]=(img_size/pl_kernel_num+1)*img_size;
	inputDataLength[pl_kernel_num-1]=inputDataLength[0];
	outputDataLength[0]=(img_size/pl_kernel_num-1)*result_size;
	outputDataLength[pl_kernel_num-1]=outputDataLength[0];
	for(int i=1;i<pl_kernel_num-1;++i){
		inputDataLength[i]=(img_size/pl_kernel_num+2)*img_size;
		outputDataLength[i]=(img_size/pl_kernel_num)*result_size;
	}


	//Allocate buffers for kernels
	std::cout<<"Allocate buffers for kernels"<<std::endl;
	xrt::bo input_buff[pl_kernel_num],output_buff[pl_kernel_num];
	for(int i=0;i<pl_kernel_num;++i){
		//input buffer
		size_t input_buff_size=sizeof(int)*inputDataLength[i];
		input_buff[i]=xrt::bo(device,input_buff_size,pl_mm2s[i].group_id(0));

		//output buffer
		size_t output_buff_size=sizeof(int)*outputDataLength[i];
		output_buff[i]=xrt::bo(device,output_buff_size,pl_s2mm[i].group_id(0));
	}
	auto conv_kernel_buff=xrt::bo(device , sizeof(conv_kernel) , conv_kernal_mm2s.group_id(0));

	//write convolutional kernel to buffer
	std::cout<<"write and synchronize convolutional kernel to device buffer"<<std::endl;
	conv_kernel_buff.write(conv_kernel);
	conv_kernel_buff.sync(XCL_BO_SYNC_BO_TO_DEVICE);
	


	//write input data to buffer
	std::cout<<"write and synchronize image to device buffer"<<std::endl;
	int* DataInput[pl_kernel_num];
	DataInput[0]=new int[inputDataLength[0]];
	memcpy(DataInput[0],img,inputDataLength[0]*sizeof(int));
	input_buff[0].write(DataInput[0]);
	input_buff[0].sync(XCL_BO_SYNC_BO_TO_DEVICE);
	std::cout<<"input_buffer_0 finished"<<std::endl;

	for(int i=1;i<pl_kernel_num;++i){
		DataInput[i]=new int[inputDataLength[i]];
		memcpy(DataInput[i],img+((img_size/pl_kernel_num)*i-1)*img_size,inputDataLength[i]*sizeof(int));
		input_buff[i].write(DataInput[i]);
		input_buff[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
		std::cout<<"input_buffer_"<<i<<" finished"<<std::endl;
	}

	//xrt::run object for pl_mm2s and pl_s2mm
	xrt::run run_pl_mm2s[pl_kernel_num],run_pl_s2mm[pl_kernel_num];

	//start AIE filter2D timing
	auto aie_start=std::chrono::steady_clock::now();

	//Execute and wait convolution kernel transmission
	//std::cout << "Run the conv_kernal_mm2s for transmission data" << std::endl;
	auto run_conv_kernal_mm2s=conv_kernal_mm2s(conv_kernel_buff,nullptr,12);
	
	//std::cout << "conv_kernal_mm2s completed" << std::endl;

	//Execute the compute units
	//std::cout << "Run the pl kernels" << std::endl;
	
	for(int i=0;i<pl_kernel_num;++i){
		run_pl_s2mm[i]=pl_s2mm[i](output_buff[i],nullptr,outputDataLength[i]);
		run_pl_mm2s[i]=pl_mm2s[i](input_buff[i],nullptr,inputDataLength[i]);
		
	}
	

	//Wait for kernels to complete
	run_conv_kernal_mm2s.wait();

	for(int i=0;i<pl_kernel_num;++i){
		run_pl_mm2s[i].wait();
		//std::cout << "pl_mm2s_"<<i+1<<" completed" << std::endl;
	}

	for(int i=0;i<pl_kernel_num;++i){
		run_pl_s2mm[i].wait();
		//std::cout << "pl_s2mm_"<<i+1<<" completed" << std::endl;
	}

	//stop AIE filter2D timing
	auto aie_stop=std::chrono::steady_clock::now();

	// Read output buffer data to local buffer
	std::cout << "Read output buffer data to aie_result" << std::endl;
	int* pointer=aie_result;
	for(int i=0;i<pl_kernel_num;++i){
		output_buff[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
		output_buff[i].read(pointer);
		pointer+=outputDataLength[i];
	}

	//start PS filter2D timing
	auto ps_start=std::chrono::steady_clock::now();

	//compute filter2D in PS
	std::cout << "compute the filter2D result: ps_result" << std::endl;
	int* ps_result=filter2D(img,img_size,conv_kernel,conv_kernal_size);

	//stop PS filter2D timing
	auto ps_stop=std::chrono::steady_clock::now();

	//compare the result
	std::cout << "compare aie_result with ps_result" << std::endl;
	int errorCount=0;
	for(unsigned int i=0;i<result_size*result_size;++i){
		if(aie_result[i] != ps_result[i]){
			std::cout << "Error found @ " << std::dec << i;
			std::cout << ", (aie_result)" << aie_result[i] << " != (ps_result)" << ps_result[i] <<std::endl;
			++errorCount;
		}
	}
	
	if(errorCount){
		std::cout << "Test failed with " << std::dec << errorCount << " errors" << std::endl;
	}
	else{
		std::cout << "TEST PASSED !"<<std::endl;
	}

	//compute the time consuming
	std::chrono::duration<double,std::milli> aie_time=aie_stop-aie_start;
	std::chrono::duration<double,std::milli> ps_time=ps_stop-ps_start;
	std::cout<<"AIE filter2D take up "<<aie_time.count()<<"ms"<<std::endl;
	std::cout<<"PS filter2D take up "<<ps_time.count()<<"ms"<<std::endl;
	std::cout<<"Speedup ratio:"<<ps_time.count()/aie_time.count()<<std::endl;

	//free the memory
	for(int i=0;i<pl_kernel_num;++i) delete DataInput[i];
	delete ps_result;

}
