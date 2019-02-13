//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//#include <llvm/CAUTH/CauthIntr.h>

#include "llvm/CAUTH/CauthIntr.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace llvm::CAUTH;

Value *CauthIntr::pacga(IRBuilder<> *builder, hasModule &M, Instruction &I, bool hasMod, const std::string &name) {
  
  auto &C = M.getContext();
  Type *arg_types[] = { Type::getInt64Ty(C) };
  auto pacIntr = Intrinsic::getDeclaration(&M, (hasMod ? Intrinsic::ca_pacga : Intrinsic::ca_pacgza));
  Value *src = Constant::getIntegerValue(Type::getInt64Ty(C), APInt(64,1000));
  if (hasMod){
    Value *hasModifier = Constant::getIntegerValue(Type::getInt64Ty(C), APInt(64,10));
    // Create the arguments for the intrinsic call (i.e., original value + hasModifier)
    Value *args[] { src, hasModifier };
    return builder->CreateCall(pacIntr, args, name);
  }
  else{
    Value *args[] { src };
    return builder->CreateCall(pacIntr, args, name);
  }
}

Value *CauthIntr::pacga(Function &F, Instruction &I, bool hasMod,const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacga(&Builder, *F.getParent(), I, hasMod, name);
}


Value *CauthIntr::pacda(IRBuilder<> *builder, hasModule &M, Value *V, bool hasMod, const std::string &name) {
  
  auto &C = M.getContext();
  Type *arg_types[] = { V->getType() };
  auto pacIntr = Intrinsic::getDeclaration(&M, (hasMod ? Intrinsic::ca_pacda : Intrinsic::ca_pacdza), arg_types);
  if (hasMod){
    Value *hasModifier = Constant::getIntegerValue(Type::getInt64Ty(C), APInt(64,10));
    // Create the arguments for the intrinsic call (i.e., original pointer + hasModifier)
    Value *args[] { V, hasModifier };
    return builder->CreateCall(pacIntr, args, name);
  }
  else{
    Value *args[] { V };
    return builder->CreateCall(pacIntr, args, name);
  }
}

Value *CauthIntr::pacda(Function &F, Instruction &I, Value *V, bool hasMod, const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pacda(&Builder, *F.getParent(), V, hasMod, name);
}

Value *CauthIntr::autda(IRBuilder<> *builder, hasModule &M, Value *V, bool hasMod, const std::string &name) {
  // Get the intrinsic declaration based on our specific pointer type
  auto &C = M.getContext();
  Type *arg_types[] = { V->getType() };
  auto intr = Intrinsic::getDeclaration(&M, (hasMod ? Intrinsic::ca_autda : Intrinsic::ca_autdza), arg_types);

 if (hasMod){
    Value *hasModifier = Constant::getIntegerValue(Type::getInt64Ty(C), APInt(64,10));
    // Create the arguments for the intrinsic call (i.e., original pointer + hasModifier)
    Value *args[] { V, hasModifier };
    return builder->CreateCall(pacIntr, args, name);
  }
  else{
    Value *args[] { V };
    return builder->CreateCall(pacIntr, args, name);
  }
}

Value *CauthIntr::autda(Function &F, Instruction &I, Value *V, bool hasMod, const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return autda(&Builder, *F.getParent(), V, hasMod, name);
}