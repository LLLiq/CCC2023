# CCFSys 2023 - First Prize - BJUT

> University：北京工业大学(BJUT)
> 
> Mentor：张文博老师(Wenbo Zhang)、包振山老师(Zhenshan Bao)
> 
> Team：刘一祺（Yiqi Liu）、王天硕（Tianshuo Wang）、李一鸣（Yiming Li）

以上信息添加英文备注

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


## Design Report

- 设计报告：[设计报告](https://github.com/LLLiq/ccc2023_liq/blob/finals/%E5%8C%97%E4%BA%AC%E5%B7%A5%E4%B8%9A%E5%A4%A7%E5%AD%A6%E6%9E%81%E5%85%89%E9%98%9F%E6%8A%80%E6%9C%AF%E6%8A%A5%E5%91%8A_%E5%86%B3%E8%B5%9B.pdf)

## Excution

以本仓库`project`文件夹下提交的`filter2d_64x64_basic`工程为例，可以执行AIE仿真和硬件运行。

1. AIE仿真

可以拷贝aie文件夹中的src和data部分到vitis中执行编译和仿真。

2. 硬件运行

 项目构建： 
 进入对应的源码文件夹使用以下命令编译
 >注：编译初始目录默认在project目录下。
```
cd filter2D_64x64_basic/aie
make all
cd ../pl
make all
cd ../host
make all
cd ..
make all
```
项目运行：
在完成项目构建后，可以使用如下命令快速开始运行：
```
cd host
./template.exe ../build.xilinx_vck5000_gen4x8_qdma_2_202220_1.hw/template.xclbin
```

执行完毕后，可使用`sources/fft_4k/notebook`文件夹下的`.ipynb`文件可视化输出结果并进行验证。

## Repository tree
本仓库的主要目录结构如下。
```
CCC2023
├── sources
│   ├── fft_4k          4K-point FFT完整代码
│   │   ├── aie
│   │   ├── execution
│   │   ├── host
│   │   ├── hw_link
│   │   ├── notebook
│   │   ├── pl
│   │   └── Makefile
│   │
│   └── fft_8k         8K-point FFT完整代码，目录结构同fft_4k
│
├── README.md
└── 设计报告.pdf
```

## Reference
