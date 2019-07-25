//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/CAuth/CAuthIntr.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace llvm::CAuth;

Value *CAuthIntr::pacga(IRBuilder<> *builder,
                        Module &M, 
                        Instruction &I, 
                        long long funcID, 
                        const std::string &name) {
  
  auto pacIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_pacga);
  Value *src = Constant::getIntegerValue(Type::getInt64Ty(M.getContext()), APInt(64,funcID));
  Value *args[] { src };
  return builder->CreateCall(pacIntr, args, name);
}

Value *CAuthIntr::pacga(Function &F,
                        Instruction &I,
                        long long funcID, 
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacga(&Builder, *F.getParent(), I, funcID, name);
  
}


Value *CAuthIntr::pacda(IRBuilder<> *builder,
                        Module &M, 
                        Value *V, 
                        const std::string &name) {
  
  Type *arg_types[] = { V->getType() };
  auto pacIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_pacda, arg_types);
  Value *args[] { V };
  return builder->CreateCall(pacIntr, args, name);
}

Value *CAuthIntr::pacda(Function &F,
                        Instruction &I, 
                        Value *V,
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacda(&Builder, *F.getParent(), V, name);
}

Value *CAuthIntr::autda(IRBuilder<> *builder,
                        Module &M, 
                        Value *V, 
                        const std::string &name) {

  Type *arg_types[] = { V->getType() };
  auto autIntr = Intrinsic::getDeclaration(&M, Intrinsic::ca_autda, arg_types);
  Value *args[] { V };
  return builder->CreateCall(autIntr, args, name);
}

Value *CAuthIntr::autda(Function &F,
                        Instruction &I, 
                        Value *V, 
                        const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return autda(&Builder, *F.getParent(), V, name);
}