//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CAUTHINTR_H
#define LLVM_CAUTHINTR_H

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

namespace llvm {

namespace CAUTH {

class CauthIntr {

public:
  static Value *pac_pointer(Function &F, Instruction &I, const std::string &name = "");
  static Value *pac_pointer(IRBuilder<> *builder, Module &M, Instruction &I, const std::string &name = "");

  static Value *load_aut_pointer(Function &F, Instruction &I);
  static Value *store_aut_pointer(Function &F, Instruction &I);
};

} // CAUTH

} // llvm

#endif //LLVM_CAUTHINTR_H