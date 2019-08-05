//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/CAuth/CAuth.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

using namespace llvm;
using namespace CAuth;

#define DEBUG_TYPE "cauth-ir"

STATISTIC(TotalFunctionCounter, "Total number of functions in code");
STATISTIC(FunctionCounter, "number of functions instrumented");
STATISTIC(ArrayBuffCounter, "number of array buffers instrumented");
STATISTIC(TotalBuffCounter, "Total number of buffers instrumented");

namespace {

struct CAuthCanaryPass : public FunctionPass {
  static char ID;
  unsigned m_funcID = 0;

  CAuthCanaryPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  static Value *instrumentEntry(Function &F, unsigned funcID);
  static bool instrumentReturn(Function &F, unsigned funcID, Value *canary);

  static BasicBlock *CreateEmptyBB(LLVMContext &C, const Twine &Name = "",
                                   Function *Parent = nullptr,
                                   BasicBlock *InsertBefore = nullptr);

  static void CreateFailBB(LLVMContext &C, Function *F, BasicBlock *FalseBB,
                           Value *save_ret);

private:
  static inline Function *getProModDecl(Function &F) {
    return Intrinsic::getDeclaration(F.getParent(), Intrinsic::cauth_pro_mod);
  }

  static inline Function *getEpiModDecl(Function &F) {
    return Intrinsic::getDeclaration(F.getParent(), Intrinsic::cauth_epi_mod);
  }

  static inline Function *getPacgaDecl(Function &F) {
    return Intrinsic::getDeclaration(F.getParent(), Intrinsic::ca_pacga);
  }

  static inline Function *getPacdaDecl(Function &F, Value *V) {
    return Intrinsic::getDeclaration(F.getParent(), Intrinsic::ca_pacda,
                                     V->getType());
  }

  static inline Function *getAutdaDecl(Function &F, Value *V) {
    return Intrinsic::getDeclaration(F.getParent(), Intrinsic::ca_autda,
                                     V->getType());
  }

  static inline Constant *getFuncIDConstant(LLVMContext &C,
                                            const unsigned funcID) {
    return Constant::getIntegerValue(Type::getInt64Ty(C),
                                     APInt(64,funcID));
  }
};

}


char CAuthCanaryPass::ID = 0;
static RegisterPass<CAuthCanaryPass> X("cauth-ir-arrays",
                                     "CAuth IR pass for protecting arrays");

Pass *CAuth::createCAuthCanaryPass() { return new CAuthCanaryPass(); }

bool CAuthCanaryPass::runOnFunction(Function &F) {
  ++TotalFunctionCounter;
  const unsigned funcID = ++m_funcID;

  F.addFnAttr("cauth-funcid", std::to_string(funcID));

  auto *canary = instrumentEntry(F, funcID);

  if (canary == nullptr)
    return false;

  ++FunctionCounter;

  bool changed = instrumentReturn(F, funcID, canary);

  assert(changed && "prologue instrumented but not the epilogue!?!");
  return changed;
}

Value *CAuthCanaryPass::instrumentEntry(Function &F, const unsigned funcID) {
  auto &C = F.getContext();
  auto &BB = F.getEntryBlock();

  Value *prevCanaryAlloca = nullptr;
  Type *buffTy = Type::getInt64Ty(C);

  Value *mod = nullptr;

  for (auto &MI : BB) {
    if (isa<AllocaInst>(MI)) {
      auto *alloca = dyn_cast<llvm::AllocaInst>(&MI);

      if (alloca->getAllocatedType()->isArrayTy()) {
        ++TotalBuffCounter;
        ++ArrayBuffCounter;
        // Instrument buffer allocations
        IRBuilder<> Builder(&MI);

        // Make sure we have a modifier
        if (mod == nullptr)
          mod = Builder.CreateCall(getProModDecl(F));

        if (prevCanaryAlloca != nullptr) // Update type unless this is first canary
          buffTy = PointerType::get(buffTy, 0);

        //prepare instruction to allocate space for signed canary
        auto *arr_alloc = Builder.CreateAlloca(buffTy, nullptr, "cauth_alloc");

        // Generate the canary
        auto *canary = prevCanaryAlloca == nullptr ?
                       Builder.CreateCall(getPacgaDecl(F), mod, "pga") :
                       Builder.CreateCall(getPacdaDecl(F, prevCanaryAlloca),
                                          { prevCanaryAlloca, mod }, "pda");

        // Store canary
        Builder.CreateAlignedStore(canary, arr_alloc, 8);

        // Keep the allocation location for next iteration
        prevCanaryAlloca = arr_alloc;
      }
    }
  }

  return prevCanaryAlloca;
}

bool CAuthCanaryPass::instrumentReturn(Function &F, const unsigned funcID,
                                       Value *canary) {
  bool changed = false;
  auto &C = F.getContext();

  Value *mod = nullptr;

  for (auto &BB : F) {
    // Skip BBs added by cauth instrumentation
    if (BB.getName().startswith_lower("cauth."))
      continue;

    for (auto Ii = BB.begin(), end = BB.end(); Ii != end; ++Ii) {
      auto &I = *Ii;

      if (isa<ReturnInst>(I)) {
        IRBuilder<> Builder(&I);

        // Make sure we have a modifier
        if (mod == nullptr)
          mod = Builder.CreateCall(getEpiModDecl(F));

        auto *rI = dyn_cast<llvm::ReturnInst>(&I);
        auto *canary_val = Builder.CreateLoad(canary);

        while (canary_val->getType() != Type::getInt64Ty(C)) {
          auto autda = Builder.CreateCall(getAutdaDecl(F, canary_val),
                                          { canary_val, mod }, "eda");

          canary_val = Builder.CreateLoad(autda);
        }

        auto *pacga = Builder.CreateCall(getPacgaDecl(F), mod, "ega");
        auto *cmp = Builder.CreateICmp(llvm::CmpInst::ICMP_EQ,
                                       canary_val, pacga, "cmp");

        // Generate BBs for auth failure and normal return
        auto TrueBB = CAuthCanaryPass::CreateEmptyBB(C, "cauth.ret", &F);
        auto FalseBB = CAuthCanaryPass::CreateEmptyBB(C, "cauth.fail", &F);

        // Conditionally jump to fail or okay return
        Builder.CreateCondBr(cmp, TrueBB, FalseBB);

        // Populate the fail BB
        CAuthCanaryPass::CreateFailBB(C, &F, FalseBB, rI->getReturnValue());

        // Make sure the okay return works
        llvm::ReturnInst::Create(C, rI->getReturnValue(), TrueBB);

        // Remove the now old return
        --Ii;
        I.eraseFromParent();

        changed = true;
      }
    }
  }

  return changed;
}

BasicBlock *CAuthCanaryPass::CreateEmptyBB(LLVMContext &C, const Twine &Name,
                                         Function *Parent,
                                         BasicBlock *InsertBefore) {
  return llvm::BasicBlock::Create(C, Name, Parent, InsertBefore);
}

// Inserts canary_chk_fail instructions into the FalseBB
void CAuthCanaryPass::CreateFailBB(LLVMContext &C, Function *F,
                                 BasicBlock *FalseBB, Value *save_ret) {
  IRBuilder<> B(FalseBB);
  Module *M = F->getParent();
  auto arg = B.CreateGlobalString(
      "\n***Canary Check Failed***\nExiting....\n\n", "__canary_chk_fail");
  // print "canary check failed" message
  Constant *printfFunc = M->getOrInsertFunction("printf", FunctionType::get(
      IntegerType::getInt32Ty(C),
      PointerType::get(Type::getInt8Ty(C), 0)));
  B.CreateCall(printfFunc, {arg}, "printfCall");
  Value *one = ConstantInt::get(Type::getInt32Ty(M->getContext()), 1);
  FunctionType *fType = FunctionType::get(Type::getVoidTy(C),
                                          Type::getInt32Ty(C), false);
  // exit
  Constant *exitF = M->getOrInsertFunction("exit", fType);
  B.CreateCall(exitF, one);
  llvm::ReturnInst::Create(C, save_ret, FalseBB);
}