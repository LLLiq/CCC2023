# Copyright (C) 2023 Advanced Micro Devices, Inc
#
# SPDX-License-Identifier: MIT

PLATFORM := xilinx_vck5000_gen4x8_qdma_2_202220_1
TARGET := hw
FREQ := 250

XSA = $(strip $(patsubst %.xpfm, % , $(shell basename $(PLATFORM))))

BUILD_DIR = build.$(XSA).$(TARGET)
VPP_FLAGS = -t $(TARGET) --platform $(PLATFORM)# --save-temps
VPP_FLAGS += --kernel_frequency $(FREQ)

kernel_list = pl_receive_border pl_receive_main pl_send_main pl_send_border krnl_jpeg
#list of .xo files
BINARY_OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .xo, $(kernel_list)))

all: $(BINARY_OBJS)
	
$(BUILD_DIR)/pl_receive_border.xo: src/pl_receive_border.cpp
	v++ $(VPP_FLAGS) -k $(subst $(BUILD_DIR)/,,$(subst .xo,,$@)) -c -g -o $@ $^
	
$(BUILD_DIR)/pl_receive_main.xo: src/pl_receive_main.cpp
	v++ $(VPP_FLAGS) -k $(subst $(BUILD_DIR)/,,$(subst .xo,,$@)) -c -g -o $@ $^
	
$(BUILD_DIR)/pl_send_main.xo: src/pl_send_main.cpp
	v++ $(VPP_FLAGS) -k $(subst $(BUILD_DIR)/,,$(subst .xo,,$@)) -c -g -o $@ $^
	
$(BUILD_DIR)/pl_send_border.xo: src/pl_send_border.cpp
	v++ $(VPP_FLAGS) -k $(subst $(BUILD_DIR)/,,$(subst .xo,,$@)) -c -g -o $@ $^

$(BUILD_DIR)/krnl_jpeg.xo: ./src/krnl_jpeg/krnl_jpeg.cpp ./src/krnl_jpeg/include/hw/jpegDec/*
	v++ $(VPP_FLAGS) -I ./src/krnl_jpeg/include/hw/jpegDec \
		--advanced.prop kernel.kernelJpegDecoder.kernel_flags="-std=c++0x -D__SDSVHLS__ -DHLS_NO_XIL_FPO_LIB" \
		-k $(subst $(BUILD_DIR)/,,$(subst .xo,,$@)) -c -g -o $@ $^

clean:
	rm -rf $(BUILD_DIR) *.log *.jou .Xil/
