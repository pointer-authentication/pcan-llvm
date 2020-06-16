# PCan

This repository contains a fork of the LLVM compiler infrastructure that implements a prototype of PCan. PCan was presented at SysTEX '19 ([link to paper](https://doi.org/10.1145/3342559.3365336)).

## Testing PCan

To use this directly without modifications, compile PCan LLVM/Clang within `PCan-LLVM/build/cauth`. Then, create symlinks from `PCan-LLVM/cauth/sysroot` and `PCan-LLVM/cauth/gcc` to the appropriate sysroot and GCC installations; or run `./install.sh` to fetch them form Linaro. 

This allows the code examples under `PCan-LLVM/cauth/test` to be compiled directly by running `make` within the folder.
