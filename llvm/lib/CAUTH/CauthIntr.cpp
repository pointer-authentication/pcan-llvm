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

Value *CauthIntr::pac_pointer(IRBuilder<> *builder, Module &M, Instruction &I, const std::string &name) {
  // Get the intrinsic declaration based on our specific pointer type
  Type *arg_types[] = { I.getType() };
  
  auto pacIntr = Intrinsic::getDeclaration(&M,
                                           (Intrinsic::ca_pacda),
                                           arg_types);

  Value *modifier = ConstantInt::get(I.getType(), 0x0);
  Value *args[] { modifier };
  return builder->CreateCall(pacIntr, args, name);
}

Value *CauthIntr::pac_pointer(Function &F, Instruction &I, const std::string &name) {
  // insert the call
  IRBuilder<> Builder(&I);
  return pac_pointer(&Builder, *F.getParent(), I, name);
}

Value *CauthIntr::load_aut_pointer(Function &F, Instruction &I) {
 
  IRBuilder<> Builder(&I);

  Value *mod = ConstantInt::get(I.getType(), 0x0);
  auto *ptr = Builder.Insert(I.clone());

  // Insert the unPAC/AUT intrinsic

  Type *arg_types[] = { I.getType() };
  //(errs
  I.dump();
  auto aut = Intrinsic::getDeclaration(F.getParent(), Intrinsic::ca_autda, arg_types);

  Value *args[] { ptr, mod };
  auto *V = Builder.CreateCall(aut, args, "unPACed_");

  // Replace uses of old instruction with new one
  I.replaceAllUsesWith(V);

  // Don't remove, optimizer should maybe get rid of this anyway?
  //I.removeFromParent();

  //return V;
  return nullptr;
}

Value *CauthIntr::store_aut_pointer(Function &F, Instruction &I) {
  
  IRBuilder<> Builder(&I);

  Value *mod = ConstantInt::get(I.getType(), 0x0);
  auto *ptr = Builder.Insert(I.clone());

  // Insert the unPAC/AUT intrinsic

  Type *arg_types[] = { I.getType() };
  auto aut = Intrinsic::getDeclaration(F.getParent(), Intrinsic::ca_pacda, arg_types);

  Value *args[] { ptr, mod };
  Builder.CreateCall(aut, args, "PACed_");

  // Replace uses of old instruction with new one
  //I.replaceAllUsesWith(V);

  // Don't remove, optimizer should maybe get rid of this anyway?
  //I.removeFromParent();

  //return V;
  return nullptr;
}