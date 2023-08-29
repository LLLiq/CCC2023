# CCFSys 2023 - Second Prize - ECNU 

> University：北京工业大学 
> 
> Mentor：张文博老师、包振山老师
> 
> Team：刘一祺、王天硕、李一鸣

以上信息添加英文备注

 ## Problem

- Basic  - 1024-Point FFT Single Kernel Programming

    The basic requirement is to complete a 1k-Point FFT design based on  personal understanding using AIE API or AIE Intrinsic. 

    - AIE Emulation succeeded
    - The design report submitted

- Advanced  - Explore very large point FFT (8k ~ 64k points) design on VCK5000 

    - The system level emulation or the hardware run on VCK5000 succeeded
    - The design report submitted

## Performance Analyzer


## Design Report

- 设计报告：[设计报告](https://github.com/abuqiqi/CCC2023/blob/main/%E8%AE%BE%E8%AE%A1%E6%8A%A5%E5%91%8A.pdf)

## Excution

以本仓库`sources`文件夹下提交的`fft_4k`工程为例，可以执行AIE仿真和硬件运行。

1. AIE仿真

执行以下代码可以单独运行AIE的仿真。
> 注：执行`make`之前需要确认`Makefile`文件中`XPFM`路径。

```shell
# 编译并运行AIE仿真
cd ./CCC2023/sources/fft_4k/aie
make
make aieemu
```

2. 硬件运行

	1. 在本地环境运行
	2. 在HACC NUX运行
在`sources/fft_4k/execution`文件夹下存放了通过主机调用PL和AIE必要的`fft.xclbin`文件、`host.exe`文件和输入文件`DataInFFTO.txt`，以及运行完毕所产生的输出文件`DataOutFFT0.txt`。如需在VCK5000上运行，可执行以下代码。

```shell
# 克隆hacc_demo仓库
git clone https://github.com/Xtra-Computing/hacc_demo.git

# 获取VCK5000计算节点（根据hacc_demo存放路径修改指令）
./hacc_demo/env/vck5000_alloc 3
source ./hacc_demo/env/vck5000_env

# 在本项目的execution文件夹下运行可执行文件
./CCC2023/sources/fft_4k/execution/host.exe

# 退出节点
./hacc_demo/env/vck5000_exit
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
