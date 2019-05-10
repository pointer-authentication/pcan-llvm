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

  static Value *pacga(Function &F, Instruction &I, long long funcID, const std::string &name = "");
  static Value *pacga(IRBuilder<> *builder, Module &M, Instruction &I, long long funcID, const std::string &name = "");

  static Value *pacda(Function &F, Instruction &I, Value *V, const std::string &name = "");
  static Value *pacda(IRBuilder<> *builder, Module &M, Value *V, const std::string &name = "");

  static Value *autda(Function &F, Instruction &I, Value *V, const std::string &name = "");
  static Value *autda(IRBuilder<> *builder, Module &M, Value *V, const std::string &name = "");
};

} // CAUTH

} // llvm

#endif //LLVM_CAUTHINTR_H