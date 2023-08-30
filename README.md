# CCFSys 2023 - First Prize - BJUT

> University：北京工业大学(BJUT)
> 
> Mentor：张文博老师(Wenbo Zhang)、包振山老师(Zhenshan Bao)
> 
> Team：刘一祺（Yiqi Liu）、王天硕（Tianshuo Wang）、李一鸣（Yiming Li）


 ## Problem

- Basic  - Vision 3X3 filter2D on 64x64 image

    Create the filter2D function with 3x3 kernel on 64x64 image using AIE API or AIE Intrinsic based on personal understanding. 

    - AIE Emulation succeeded
    - The design report submitted

- Advanced  - Image Signal Processing Pipeline on HD image

    Create high definition image processing pipeline design based on personal understanding. It is encouraged to fully use the Vitis Vision Library.

    - The system level emulation or the hardware run on VCK5000 succeeded
    - The design report submitted
      
## Performance Analyzer

比赛期间完成了三种分辨率图像的filter2d设计。在初赛阶段完成了64x64分辨率以及1280x720分辨率的filter2d设计，决赛阶段优化了初赛代码以及实现了8K分辨率的filter2d设计。

本项目整体完成情况如下。

- 完成了8K分辨率int32类型的filter2d算法的AIE设计、优化和仿真；
- 完成了AIE、PL、host三端配合，能够在VCK5000硬件上运行。
- 实现了在int32类型的8K分辨率下平均101.97FPS的运行速度。

## Design Report

- 设计报告：[设计报告](https://github.com/LLLiq/ccc2023_liq/blob/finals/%E5%8C%97%E4%BA%AC%E5%B7%A5%E4%B8%9A%E5%A4%A7%E5%AD%A6%E6%9E%81%E5%85%89%E9%98%9F%E6%8A%80%E6%9C%AF%E6%8A%A5%E5%91%8A_%E5%86%B3%E8%B5%9B.pdf)

## Excution

1. AIE仿真

考虑到仿真效率和时间，可以使用仓库`project`文件夹下提交的`filter2d_64x64_basic`工程即可执行AIE仿真。
拷贝aie文件夹中的src和data分到vitis中执行编译和仿真。

2. 硬件运行

硬件运行使用仓库`project`文件夹下提交的`filter2d_8K_advanced`工程硬件运行。
在每个工程目录中都有`execution`文件夹，此文件夹下存放了编译好的xclbin文件，如需在VCK5000上运行，可执行以下代码。

```shell
# 克隆仓库
git clone https://github.com/LLLiq/CCC2023/tree/main

# 编译host端
cd project/filter2d_8K_advanced/host
make all

# 执行
./template.exe ../execution/template.xclbin
```

执行完毕后，控制台输出的结果即可验证本次执行的输出结果以及统计运行时间。

## Repository tree
本仓库的主要目录结构如下。
```
CCC2023
├── project
│   ├── filter2d_8k_advanced          8K分辨率filter2d完整代码
│   │   ├── aie
│   │   ├── execution
│   │   ├── host
│   │   ├── hw_link
│   │   ├── pl
│   │   └── Makefile
│   │
│   └── filter2d_64x64_basic         64x64分辨率filter2d完整代码，目录结构同filter2d_8k
│   └── filter2d_1280x720_basic      1280x720分辨率filter2d完整代码，目录结构同filter2d_8k
├── notebook
│   ├── fft_sw.ipynb
│   ├── fft_aie.ipynb
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

