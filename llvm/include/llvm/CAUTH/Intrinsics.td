//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

def int_ca_pacgza : Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_ca_pacdza : Intrinsic<[llvm_anyptr_ty], [LLVMMatchType<0>, llvm_i64_ty], [IntrNoMem]>;
def int_ca_autdza : Intrinsic<[llvm_anyptr_ty], [LLVMMatchType<0>, llvm_i64_ty], [IntrNoMem]>;

def int_ca_pacga : Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_ca_pacda : Intrinsic<[llvm_anyptr_ty], [LLVMMatchType<0>], [IntrNoMem]>;
def int_ca_autda : Intrinsic<[llvm_anyptr_ty], [LLVMMatchType<0>], [IntrNoMem]>;

