#include <cstring>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iomanip>
#include <chrono>
#include <ratio>

#include <opencv2/imgcodecs.hpp>

#include <sys/stat.h>
#include <fstream>

#include "utils_XAcc_jpeg.hpp"

#include <xrt/xrt_bo.h>
#include <xrt/xrt_kernel.h>
#include "experimental/xrt_kernel.h"
#include "experimental/xrt_uuid.h"

#define yuv_buffer_size 			MAXCMP_BC*64

// Kernel arguments index definition
#define krnl_jpeg_jpeg_ptr          0
#define krnl_jpeg_jpeg_size         1
#define krnl_jpeg_yuv_ptr           2
#define krnl_jpeg_infos_ptr         3

typedef int data_t;

//filter2D algorithm on PS
int* filter2D(int* img, unsigned int img_width, unsigned int img_length, const int* kernel, unsigned int kernel_size) {
	//compute the result size
	unsigned int result_length=img_length-kernel_size+1;
	unsigned int result_width=img_width-kernel_size+1;
	//Allocate memory for result
	int* result = new int[result_length * result_width];
	memset(result, 0, sizeof(int) * result_length * result_width);
	//compute the result
	for (unsigned int row = 0; row < result_width; ++row) {
		for (unsigned int col = 0; col < result_length; ++col) {
			unsigned int iter_kernel = 0;
			//compute the result[row][column]
			for (unsigned int iter_r = 0; iter_r < kernel_size; ++iter_r) {
				for (unsigned int iter_c = 0; iter_c < kernel_size; ++iter_c) {
					result[row * result_length + col] += img[(row + iter_r) * img_length + col + iter_c] * kernel[iter_kernel++];
				}
			}
		}
	}
	return result;
}

// function to read JPEG image as binary file
void read_file(const char *file_name, int file_size, char *read_buffer)
{
    // read jpeg image as binary file to host memory
    std::ifstream input_file(file_name, std::ios::in | std::ios::binary);
    input_file.read(read_buffer, file_size);
    input_file.close();
}

// extract image information from output of JPEG decoder kernel
void rebuild_infos(xf::codec::img_info& img_info,
                   xf::codec::cmp_info cmp_info[MAX_NUM_COLOR],
                   xf::codec::bas_info& bas_info,
                   int& rtn,
                   int& rtn2,
                   uint32_t infos[1024]) {

    img_info.hls_cs_cmpc = *(infos + 0);
    img_info.hls_mcuc = *(infos + 1);
    img_info.hls_mcuh = *(infos + 2);
    img_info.hls_mcuv = *(infos + 3);

    rtn = *(infos + 4);
    rtn2 = *(infos + 5);


    bas_info.all_blocks = *(infos + 10);

    for (int i = 0; i < MAX_NUM_COLOR; i++) {
        bas_info.axi_height[i] = *(infos + 11 + i);
    }

    for (int i = 0; i < 4; i++) {
        bas_info.axi_map_row2cmp[i] = *(infos + 14 + i);
    }

    bas_info.axi_mcuv = *(infos + 18);
    bas_info.axi_num_cmp = *(infos + 19);
    bas_info.axi_num_cmp_mcu = *(infos + 20);

    for (int i = 0; i < MAX_NUM_COLOR; i++) {
        bas_info.axi_width[i] = *(infos + 21 + i);
    }

    int format = *(infos + 24);
    bas_info.format = (xf::codec::COLOR_FORMAT)format;

    for (int i = 0; i < MAX_NUM_COLOR; i++) {
        bas_info.hls_mbs[i] = *(infos + 25 + i);
    }

    bas_info.hls_mcuc = *(infos + 28);

    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bas_info.idct_q_table_x[c][i][j] = *(infos + 29 + c * 64 + i * 8 + j);
            }
        }
    }
    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bas_info.idct_q_table_y[c][i][j] = *(infos + 221 + c * 64 + i * 8 + j);
            }
        }
    }
    
    bas_info.mcu_cmp = *(infos + 413);

    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        for (int i = 0; i < 64; i++) {
            bas_info.min_nois_thld_x[c][i] = *(infos + 414 + c * 64 + i);
        }
    }
    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        for (int i = 0; i < 64; i++) {
            bas_info.min_nois_thld_y[c][i] = *(infos + 606 + c * 64 + i);
        }
    }
    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bas_info.q_tables[c][i][j] = *(infos + 798 + c * 64 + i * 8 + j);
            }
        }
    }

    for (int c = 0; c < MAX_NUM_COLOR; c++) {
        cmp_info[c].bc = *(infos + 990 + c * 6);
        cmp_info[c].bch = *(infos + 991 + c * 6);
        cmp_info[c].bcv = *(infos + 992 + c * 6);
        cmp_info[c].mbs = *(infos + 993 + c * 6);
        cmp_info[c].sfh = *(infos + 994 + c * 6);
        cmp_info[c].sfv = *(infos + 995 + c * 6);
    }

}

void rebuild_y_image(uint8_t* yuv_mcu_pointer, int mcu_height, int mcu_width, int *img){
	// ofstream stream;
	// stream.open("rebuild.log",ios::trunc);

	//get the img width and img height
    int image_height = mcu_height * 8;
	int image_width = mcu_width * 8;

	//traverse each pixel
	int pix_row_num,pix_col_num;
	int mcu_index,mcu_pix_index;
	for(int pix_iter=0; pix_iter<image_width*image_height; ++pix_iter){
		pix_row_num=pix_iter/image_width;
		pix_col_num=pix_iter%image_width;

		//comput the mcu index
		int temp_mcu_row = pix_row_num/8;
		int temp_mcu_col = pix_col_num/8;
		mcu_index = temp_mcu_row*mcu_width + temp_mcu_col;

		//compute the mcu pix index
		int temp_mcu_pix_row = pix_row_num%8;
		int temp_mcu_pix_col = pix_col_num%8;
		mcu_pix_index = temp_mcu_pix_col*8 + temp_mcu_pix_row;

		//compute the pix index
		int yuv_mcu_index = 3*64*mcu_index + mcu_pix_index;
		
		//assign the img pixel value
		img[pix_iter] = yuv_mcu_pointer[yuv_mcu_index];
	}
}

#define WIDTH 7680
#define HEIGHT 4320
#define PART_NUMBER 18 
#define PART_LINE_NUMBER HEIGHT/PART_NUMBER
#define RECEIVE_PART_NUMBER 2



int main(int argc,char** argv){

	std::cout<<"Image WIDTH:"<<WIDTH<<std::endl;
	std::cout<<"Image HEIGHT:"<<HEIGHT<<std::endl;

	const int sourceImgSize = WIDTH*HEIGHT;
	const int resultImgSize = (WIDTH-2)*(HEIGHT-2);


	//filter2D kernel
	const data_t conv_kernel[12] = {0, -2, 0,
									-2, 12, -2,
							 		0, -2, 0,
									0, 0, 0};
	
	
	//Get device index and load xclbin
	std::cout<<"open the device"<<std::endl;
	auto device=xrt::device(0);
	std::string binaryFile = argv[1];
	std::cout<<"load the xclbin "<<binaryFile<<std::endl;
	auto uuid = device.load_xclbin(binaryFile);

	//======================================================================================
	//					decode jpeg to YUV data with vitis library
	//=====================================================================================

	//pl jpeg decoder
	std::string decoder_kernel_name="krnl_jpeg";
	auto krnl_jpeg=xrt::kernel(device,uuid,decoder_kernel_name);

	// --------------------------------------------------------------------------------------
	//check input JPEG file size
	// --------------------------------------------------------------------------------------
	std::string imageName="image/img.jpg";
	if(argc > 2){ //if input image name
		imageName = argv[2];
	}

	struct stat statbuff;
    if (stat(imageName.c_str(), &statbuff)) {
        std::cout << "Cannot open file " << imageName << std::endl;
        return EXIT_FAILURE;
    }
    int file_size = statbuff.st_size;
    std::cout << "Input JPEG file size = " << file_size << std::endl;

	// --------------------------------------------------------------------------------------
	//check input JPEG file size
	// --------------------------------------------------------------------------------------
	// create host buffer for host-device data exchange
    uint8_t *jpeg_data;    // host buffer for input JPEG file
    uint8_t *yuv_data;     // host buffer for decoded YUV planner image data
    uint8_t *infos_data;   // host buffer for JPEG file information packet

	jpeg_data  = new uint8_t [file_size];
	yuv_data   = new uint8_t [yuv_buffer_size];
    infos_data = new uint8_t [4096];


	// create jpeg decoder device buffer objects
    std::cout << "Create input and output device buffers for JPEG decoder" << std::endl;
    xrt::bo jpeg_buffer  = xrt::bo (device, file_size,    xrt::bo::flags::normal, krnl_jpeg.group_id (krnl_jpeg_jpeg_ptr));
    xrt::bo yuv_buffer   = xrt::bo (device, MAXCMP_BC*64, xrt::bo::flags::normal, krnl_jpeg.group_id (krnl_jpeg_yuv_ptr));
    xrt::bo infos_buffer = xrt::bo (device, 4096,         xrt::bo::flags::normal, krnl_jpeg.group_id (krnl_jpeg_infos_ptr));

	// create JPEG kernel runner instance
    std::cout << "Create runner" << std::endl;
    xrt::run run_jpeg(krnl_jpeg); 

	// set jpeg kernel arguments
    run_jpeg.set_arg(krnl_jpeg_jpeg_ptr,    jpeg_buffer);   
    run_jpeg.set_arg(krnl_jpeg_jpeg_size,   file_size);    
    run_jpeg.set_arg(krnl_jpeg_yuv_ptr,     yuv_buffer);    
    run_jpeg.set_arg(krnl_jpeg_infos_ptr,   infos_buffer);

	std::cout << "Read JPEG file and transfer JPEG data to device"<<std::endl;
    read_file(imageName.c_str(), file_size, (char*)jpeg_data);  
    jpeg_buffer.write(jpeg_data);
    jpeg_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Run krnl_jpeg kernel"<<std::endl;
    run_jpeg.start();
    run_jpeg.wait();

    std::cout << "Transfer YUV data to host"<<std::endl;
    yuv_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    yuv_buffer.read(yuv_data);

	std::cout << "Transfer Info data to host"<<std::endl;
    infos_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    infos_buffer.read(infos_data);

	// --------------------------------------------------------------------------------------
	//extract JPEG decoder return information
	// --------------------------------------------------------------------------------------
    xf::codec::cmp_info cmp_info[MAX_NUM_COLOR];
    xf::codec::img_info img_info;
    xf::codec::bas_info bas_info;

    // 0: decode jfif successful
    // 1: marker in jfif is not in expectation
    int rtn = 0;

    // 0: decode huffman successful
    // 1: huffman data is not in expectation
    int rtn2 = false;

    rebuild_infos(img_info, cmp_info, bas_info, rtn, rtn2, (uint32_t*)infos_data);

	//======================================================================================
	//					transform the YUV to gray scale and rebuild image
	//=====================================================================================
	//get the img width and img height
	int image_width = bas_info.axi_width[0] * 8;
    int image_height = bas_info.axi_height[0] * 8; 
    
    
	if(image_width!=WIDTH || image_height!=HEIGHT){
		std::cout<<"ERROR! Incorrect image size"<<std::endl;
		return -1;
	}

	//allocate memory for gray image
	std::cout<<"Allocate memory for the image"<<std::endl;
	//int img[image_width*image_height];
	data_t *img = new data_t[sourceImgSize];

	rebuild_y_image(yuv_data, bas_info.axi_height[0], bas_info.axi_width[0],img);

	// Get reference to the PL kernels
	std::cout<<"get Kernel Reference"<<std::endl;

	xrt::kernel pl_send_border[2],pl_send_main[16];
	xrt::kernel pl_receive_border,pl_receive_main[8];
	
	std::string kernel_name;

	pl_send_border[0]=xrt::kernel(device,uuid,"pl_send_border:{pl_send_border_1}");
	pl_send_border[1]=xrt::kernel(device,uuid,"pl_send_border:{pl_send_border_2}");
	for(int i=0;i<16;i++){
		kernel_name="pl_send_main:{pl_send_main_"+std::to_string(i+1)+'}';
		pl_send_main[i]=xrt::kernel(device,uuid,kernel_name);
	}

	pl_receive_border=xrt::kernel(device,uuid,"pl_receive_border");
	for(int i=0;i<8;i++){
		kernel_name="pl_receive_main:{pl_receive_main_"+std::to_string(i+1)+'}';
		pl_receive_main[i] = xrt::kernel(device,uuid,kernel_name);
	}



	std::cout<<"calculate the required data length for each section"<<std::endl;
	const int INPUT_LENGTH_T1 = (PART_LINE_NUMBER+1)*WIDTH + 12;
	const int INPUT_LENGTH_T2 = (PART_LINE_NUMBER+2)*WIDTH + 12;
	
	const int PL_OUTPUT_LENGTH_T1 = (PART_LINE_NUMBER-1)*(WIDTH)*2;
	const int PL_OUTPUT_LENGTH_T2 = (PART_LINE_NUMBER)*(WIDTH)*2;

	int inputDataLength[PART_NUMBER];
	inputDataLength[0]=INPUT_LENGTH_T1;
	inputDataLength[PART_NUMBER-1]=INPUT_LENGTH_T1;
	for(int i=1;i<PART_NUMBER-1;i++){
		inputDataLength[i]=INPUT_LENGTH_T2;
	}

	std::cout<<"split the image and put into PL input buffers"<<std::endl;

	data_t* DataInput[PART_NUMBER];

	for(int i=0;i<PART_NUMBER;i++){
		DataInput[i] = new data_t[inputDataLength[i]];
		memcpy(DataInput[i],conv_kernel,sizeof(data_t)*12);
	}

	memcpy(12+DataInput[0],img,inputDataLength[0]*sizeof(data_t));
	for(int i=1;i<PART_NUMBER;++i){
		memcpy(12+DataInput[i],img+((PART_LINE_NUMBER)*i-1)*WIDTH,inputDataLength[i]*sizeof(data_t));
	}


	std::cout<<"allocate xrt buffers for kernels"<<std::endl;
	xrt::bo send_border_buf[2],send_main_buf[16];
	xrt::bo receive_border_buf,receive_main_buf[8];

	send_border_buf[0] = xrt::bo(device, sizeof(data_t)*(INPUT_LENGTH_T1), pl_send_border[0].group_id(0));
	send_border_buf[1] = xrt::bo(device, sizeof(data_t)*(INPUT_LENGTH_T1), pl_send_border[1].group_id(0));
	for(int i=0;i<16;i++){
		send_main_buf[i]=xrt::bo(device,sizeof(data_t)*(INPUT_LENGTH_T2),pl_send_main[i].group_id(0));
	}

	receive_border_buf = xrt::bo(device, sizeof(data_t)*PL_OUTPUT_LENGTH_T1, pl_receive_border.group_id(0));
	for(int i=0;i<8;i++){
		receive_main_buf[i] = xrt::bo(device,sizeof(data_t)*PL_OUTPUT_LENGTH_T2, pl_receive_main[i].group_id(0));
	}


	
	std::cout<<"write and synchronize data to device buffer"<<std::endl;
	send_border_buf[0].write(DataInput[0]);
	send_border_buf[0].sync(XCL_BO_SYNC_BO_TO_DEVICE);

	send_border_buf[1].write(DataInput[PART_NUMBER-1]);
	send_border_buf[1].sync(XCL_BO_SYNC_BO_TO_DEVICE);

	for(int i=0;i<16;i++){
		send_main_buf[i].write(DataInput[i+1]);
		send_main_buf[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
	}
		
	std::cout<<"delete dataInput buffers"<<std::endl;
	for(int i=0;i<PART_NUMBER;i++){
		delete DataInput[i];
	}
	
	

	xrt::run run_pl_send_border[2],run_pl_send_main[16];
	xrt::run run_pl_receive_border,run_pl_receive_main[8];

	std::cout<<"running..."<<std::endl;
	auto aie_start=std::chrono::steady_clock::now();

	run_pl_send_border[0] = pl_send_border[0](send_border_buf[0],nullptr);
	run_pl_send_border[1] = pl_send_border[1](send_border_buf[1],nullptr);
	for(int i=0;i<16;i++){
		run_pl_send_main[i]=pl_send_main[i](send_main_buf[i],nullptr);
	}

	run_pl_receive_border = pl_receive_border(receive_border_buf,nullptr);
	for(int i=0;i<8;i++){
		run_pl_receive_main[i] = pl_receive_main[i](receive_main_buf[i],nullptr);
	}

	run_pl_send_border[0].wait();
	run_pl_send_border[1].wait();
	for(int i=0;i<16;i++){
		run_pl_send_main[i].wait();
	}

	run_pl_receive_border.wait();
	for(int i=0;i<8;i++){
		run_pl_receive_main[i].wait();
	}
	
	auto aie_stop=std::chrono::steady_clock::now();
	std::cout<<"run completed"<<std::endl;

	

	
	std::cout<<"allocate buffers for PL output"<<std::endl;
	data_t *mem_for_border = new data_t[PL_OUTPUT_LENGTH_T1];
	data_t *mem_for_main[8];
	for(int i=0;i<8;i++){
		mem_for_main[i] = new data_t[PL_OUTPUT_LENGTH_T2];
	}
	std::cout << "read PL output data to PS buffer" << std::endl;
	receive_border_buf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
	receive_border_buf.read(mem_for_border);
	for(int i=0;i<8;i++){
		receive_main_buf[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
		receive_main_buf[i].read(mem_for_main[i]);
	}
	
		
	std::cout<<"calculate the store data length and index for each section"<<std::endl;
	int partStoreNumber[PART_NUMBER];
	int partStoreidx[PART_NUMBER];	
	//compute output data length
	partStoreNumber[0] = (PART_LINE_NUMBER-1)*(WIDTH-2);
	partStoreNumber[PART_NUMBER-1] = (PART_LINE_NUMBER-1)*(WIDTH-2);
	for(int i=1;i<PART_NUMBER-1;++i){
		partStoreNumber[i] = (PART_LINE_NUMBER)*(WIDTH-2);
	}
	//compute part store idx
	partStoreidx[0] = 0;
	for(int i=1;i<PART_NUMBER;i++){
		partStoreidx[i] = partStoreidx[i-1] + partStoreNumber[i-1];
	}

	
	data_t *aie_result = new data_t[resultImgSize];
	memset(aie_result,0,sizeof(data_t)*resultImgSize);

	std::cout<<"reshape and merge PL output to full image"<<std::endl;
	for(int i=0;i<(PART_LINE_NUMBER-1);i++){
		for(int j=0;j<WIDTH/8;j++){
			int s_num=8,flag1=0,flag2=0;
			if(j==0){
				s_num = 7; flag1 = 1;flag2 = 0;
			}
			if(j==WIDTH/8 - 1){
				s_num = 7; flag1 = 0;flag2 = 1;
			}
			if(flag1)mem_for_border++;
			memcpy(aie_result+partStoreidx[0],mem_for_border,sizeof(data_t)*s_num);
			partStoreidx[0] += s_num;
			mem_for_border += s_num;
			if(flag2)mem_for_border++;
			
			if(flag1)mem_for_border++;
			memcpy(aie_result+partStoreidx[PART_NUMBER-1],mem_for_border,sizeof(data_t)*s_num);
			partStoreidx[PART_NUMBER-1] += s_num;
			mem_for_border += s_num;
			if(flag2)mem_for_border++;
		}
	}
	for(int mem_cnt = 0;mem_cnt<8;mem_cnt++){
		for(int i=0;i<(PART_LINE_NUMBER);i++){
			for(int j=0;j<WIDTH/8;j++){
				int s_num=8,flag1=0,flag2=0;
				if(j==0){
					s_num = 7; flag1 = 1;flag2 = 0;
				}
				if(j==WIDTH/8 - 1){
					s_num = 7; flag1 = 0;flag2 = 1;
				}
				if(flag1)mem_for_main[mem_cnt]++;
				memcpy(aie_result+partStoreidx[mem_cnt*2+1],mem_for_main[mem_cnt],sizeof(data_t)*s_num);
				partStoreidx[mem_cnt*2+1] += s_num;
				mem_for_main[mem_cnt] += s_num;
				if(flag2)mem_for_main[mem_cnt]++;
				
				if(flag1)mem_for_main[mem_cnt]++;
				memcpy(aie_result+partStoreidx[mem_cnt*2+2],mem_for_main[mem_cnt],sizeof(data_t)*s_num);
				partStoreidx[mem_cnt*2+2] += s_num;
				mem_for_main[mem_cnt] += s_num;
				if(flag2)mem_for_main[mem_cnt]++;
			}
		}
	}
	
	
	std::cout << "compute the filter2D result: ps_result" << std::endl;
	auto ps_start=std::chrono::steady_clock::now();
	int* ps_result=filter2D(img,HEIGHT,WIDTH,conv_kernel,3);
	auto ps_stop=std::chrono::steady_clock::now();

	//compare the result
	std::cout << "compare aie_result with ps_result" << std::endl;
	int errorCount=0;
	for(unsigned int i=0;i<resultImgSize;++i){
		if(aie_result[i] != ps_result[i]){
			//std::cout << "Error found @ " << std::dec << i;
			//std::cout << ", (aie_result)" << aie_result[i] << " != (ps_result)" << ps_result[i] <<std::endl;
			++errorCount;
		}
	}
	
	if(errorCount)
		std::cout << "Test failed with " << std::dec << errorCount << " errors" << std::endl;
	else
		std::cout << "TEST PASSED !"<<std::endl;


	//compute the time consuming
	std::chrono::duration<double,std::milli> aie_time=aie_stop-aie_start;
	std::chrono::duration<double,std::milli> ps_time=ps_stop-ps_start;
	std::cout<<"VCK5000 filter2D take up "<<aie_time.count()<<"ms"<<std::endl;
	std::cout<<"PS filter2D take up "<<ps_time.count()<<"ms"<<std::endl;
	std::cout<<"Speedup ratio:"<<ps_time.count()/aie_time.count()<<std::endl;
	double FPS = 1000/aie_time.count();
	std::cout<<"Frames Per Second:"<<FPS<<" FPS"<<std::endl;
	
	cv::Mat image = cv::Mat(HEIGHT-2, WIDTH-2, CV_32SC1, aie_result);
	cv::imwrite("output.jpg", image); 
	std::cout << "write output.jpg image finish!"<<std::endl;

}
