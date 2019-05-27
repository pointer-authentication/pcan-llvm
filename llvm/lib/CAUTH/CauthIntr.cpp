//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/CAUTH/CauthIntr.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace llvm::CAUTH;

Value *CauthIntr::pacga(IRBuilder<> *builder, 
                        Module &M, 
                        Instruction &I, 
                        long long funcID, 
                        const std::string &name) {
  
  auto &C = M.getContext();
  Type *arg_types[] = { Type::getInt64Ty(C) };
  auto pacIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_pacga);
  Value *src = Constant::getIntegerValue(Type::getInt64Ty(C), APInt(64,funcID));
  Value *args[] { src };
  return builder->CreateCall(pacIntr, args, name);
}

Value *CauthIntr::pacga(Function &F, 
                        Instruction &I,
                        long long funcID, 
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacga(&Builder, *F.getParent(), I, funcID, name);
  
}


Value *CauthIntr::pacda(IRBuilder<> *builder, 
                        Module &M, 
                        Value *V, 
                        const std::string &name) {
  
  auto &C = M.getContext();
  Type *arg_types[] = { V->getType() };
  auto pacIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_pacda, arg_types);
  Value *args[] { V };
  return builder->CreateCall(pacIntr, args, name);
}

Value *CauthIntr::pacda(Function &F, 
                        Instruction &I, 
                        Value *V,
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacda(&Builder, *F.getParent(), V, name);
}

Value *CauthIntr::autda(IRBuilder<> *builder, 
                        Module &M, 
                        Value *V, 
                        const std::string &name) {

  auto &C = M.getContext();
  Type *arg_types[] = { V->getType() };
  auto autIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_autda, arg_types);
  Value *args[] { V };
  return builder->CreateCall(autIntr, args, name);
}

Value *CauthIntr::autda(Function &F, 
                        Instruction &I, 
                        Value *V, 
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return autda(&Builder, *F.getParent(), V, name);
}