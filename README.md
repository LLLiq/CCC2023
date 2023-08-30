# CCFSys 2023 - First Prize - BJUT

> University：北京工业大学(BJUT)
> 
> Mentor：张文博老师(Wenbo Zhang)、包振山老师(Zhenshan Bao)
> 
> Team：刘一祺（Yiqi Liu）、王天硕（Tianshuo Wang）、李一鸣（Yiming Li）


 ## Problem

- Basic  - Perform 3X3 filter2D operation on 64x64 image

    Implement a customized 3x3 kernel size filter2D operation using AIE API or AIE Intrinsic for processing a 64x64 image,based on personal comprehension.

    - AIE Emulation succeeded
    - The design report submitted

- Advanced  - Image Signal Processing Pipeline on HD image

    Design and implement the High-Definition(HD) image processing pipeline based on personal comprehension,which is encouraged to leverage the Vitis Vision Library.

    - The system-level emulation or the hardware run on VCK5000 succeeded
    - The design report submitted
      
## Performance Analyzer

During the competition, filter2D designs were completed in three different image resolutions. In the preliminary stage, designs were created for 64x64 resolution and 1280x720 resolution. In the final stage, the preliminary code was optimized, resulting in the implementation of a filter2D design for 8K resolution.

The overall completion of the project is as follows.

- The AIE design, optimization and simulation of 8K resolution int32 filter2d algorithm are completed.
- AIE, PL, and host are completed, which can run on VCK5000 hardware.
- It achieves an average running speed of 101.97FPS at 8K resolution of int32 type.

## Design Report

- Design report：[Design report](https://github.com/LLLiq/ccc2023_liq/blob/finals/%E5%8C%97%E4%BA%AC%E5%B7%A5%E4%B8%9A%E5%A4%A7%E5%AD%A6%E6%9E%81%E5%85%89%E9%98%9F%E6%8A%80%E6%9C%AF%E6%8A%A5%E5%91%8A_%E5%86%B3%E8%B5%9B.pdf)

## Execution

1. AIE simulation

Considering the simulation efficiency and time, AIE simulation can be performed using the filter2d_64x64_basic project submitted in the repository 'project' folder.

Copy src and data from the aie folder to vitis for compilation and simulation.

2. Hardware execution

Hardware execution can be run using filter2d_8K_advanced project hardware which is submitted in the repository's project folder.

In each project directory, there is an 'execution' folder that contains the compiled xclbin file. If you want to run it on the VCK5000, you can execute the following code.

```shell
# Clone warehouse
git clone https://github.com/LLLiq/CCC2023/tree/main

# Compile host side
cd project/filter2d_8K_advanced/host
make all

# execute
./template.exe ../execution/template.xclbin
```

After the execution is completed, the output result of the console can verify the output result of the execution and calculate the running time.

## Repository tree
The directory structure of the repository is as follows.
```
CCC2023
├── project
│   ├── filter2d_8k_advanced          8K filter2d source code
│   │   ├── aie
│   │   ├── execution
│   │   ├── host
│   │   ├── hw_link
│   │   ├── pl
│   │   └── Makefile
│   └── filter2d_64x64_basic         64x64 filter2d source code，The directory structure is the same as filter2d_8k
│   └── filter2d_1280x720_basic      1280x720 filter2d source code，The directory structure is the same as filter2d_8k
├── notebook
│   ├── image
│   ├── notebook.md
└── README.md
```

## Reference
[1] AMD/Xilinx. Versal ACAP AI Engine Architecture Manual (AM009)

[2] AMD/Xilinx. AI Engine Tools and Flows User Guide (UG1076) 

[3] AMD/Xilinx. AI Engine Intrinsics User Guide

[4] AMD/Xilinx. AI Engine Kernel and Graph Programming Guide (UG1079)

[5] AMD/Xilinx. Vivado Design Suite User Guide High-Level Synthesis (UG902)

[6] AMD/Xilinx. XRT Native Library C++ API

[7] AMD/Xilinx. XRT and Vitis™ Platform Overview

[8] AMD/Xilinx. Vitis Unified Software Platform Documentation: Application Acceleration Development (UG1393)

[9] HACC@NUS. Heterogeneous Accelerated Compute Cluster (HACC) at NUS

[10] Slurm. Slurm workload manager Version23.02

[11]	ZHANG W, WANG T, LIU Y, et al. New Filter2D Accelerator on the Versal Platform Powered by the AI Engine [Z]. The 15th CCF International Symposium on Advanced Parallel Processing Technology(APPT). NANCHANG, CN. 2023

