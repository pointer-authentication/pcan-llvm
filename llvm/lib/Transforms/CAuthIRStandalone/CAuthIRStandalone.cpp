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

using namespace llvm;
using namespace CAuth;

#define DEBUG_TYPE "cauth-ir:\t"

STATISTIC(TotalFunctionCounter, "Total number of functions in code");
STATISTIC(FunctionCounter, "number of functions instrumented");
STATISTIC(ArrayBuffCounter, "number of array buffers instrumented");
STATISTIC(TotalBuffCounter, "Total number of buffers instrumented");

namespace {
  // CAuthIR - 
  struct CAuthIR : public FunctionPass {
    
  public:
    static char ID; // Pass identification
    unsigned funcID=0; 

    CAuthIR() : FunctionPass(ID) {}
    BasicBlock* CreateEmptyBB(LLVMContext &C, const Twine &Name="", 
                              Function *Parent=nullptr, BasicBlock *InsertBefore=nullptr );
    void CreateFailBB(LLVMContext &C, Function *F, BasicBlock *FalseBB, Value *save_ret);

    bool runOnFunction(Function &F) override {
      ++TotalFunctionCounter;
      ++funcID;
      unsigned numBuffs = 0;
      Value* oldcbuff = nullptr;
      auto &C = F.getParent()->getContext();
      BasicBlock* TrueBB=nullptr;
      BasicBlock* FalseBB=nullptr;
      Value *save_ret = nullptr;
      for (auto &BB : F){
        for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I){
          // look for alloca instruction within entry basic block
          if(isa<AllocaInst>(*I) && BB.getName()=="entry"){
            llvm::AllocaInst *aI = dyn_cast<llvm::AllocaInst>(&*I);
            IRBuilder<> Builder(&*I);
            unsigned i = 0;
            Type* buffTy = Type::getInt64Ty(C);
            // insert first canary 
            if (numBuffs==0){
              //prepare instruction to allocate space for signed canary
              auto arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "cauth_alloc");
              ++numBuffs;
              ++TotalBuffCounter;
              ++FunctionCounter;
              // add pacga intrinsic
              auto pacga_instr = CAuthIntr::pacga(F, *I, funcID);
              oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
              // store the signed canary
              Builder.CreateAlignedStore(pacga_instr, arr_alloc, 8);                
            }
            // check for array allocations
            else if(aI->getAllocatedType()->isArrayTy()){
              unsigned i = 0;
              while (i <= numBuffs){
                if (i==0){
                  buffTy = Type::getInt64Ty(C);
                }else{
                  auto tmp = PointerType::get(buffTy, 0);
                  buffTy = tmp;
                }
                i++;
              }
              if (numBuffs>=1){
                //prepare instruction to allocate space for signed canary
                auto arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "cauth_alloc");
                ++numBuffs;
                ++TotalBuffCounter;
                ++ArrayBuffCounter;
                // add pacda intrinsic
                auto pacda_instr = CAuthIntr::pacda(F, *I, oldcbuff);
                oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
                // store the signed canary
                Builder.CreateAlignedStore(pacda_instr, oldcbuff, 8);
              }
            }
          }
          // check if the return instruction is encountered and one or more signed canaries were added
          else if(isa<ReturnInst>(I) && numBuffs>0){
            IRBuilder<> Builder(&*I);
            llvm::ReturnInst* rI = dyn_cast<llvm::ReturnInst>(&*I);
            // load the signed canary
            auto canary_val = Builder.CreateLoad(oldcbuff);
            for (int i=numBuffs; i>0; i--){
              if (i == 1){
                // regenerate the correct pacga canary for comparison
                auto pacga2_instr = CAuthIntr::pacga(F, *I, funcID);
                auto cmp = Builder.CreateICmp(llvm::CmpInst::ICMP_EQ, canary_val, pacga2_instr, "cmp");
                TrueBB= CAuthIR::CreateEmptyBB(C, "TrueBB", &F);
                // add basic block to handle canary check failure
                FalseBB= CAuthIR::CreateEmptyBB(C, "FalseBB", &F);
                Builder.CreateCondBr(cmp, TrueBB, FalseBB);
                // save original return value
                save_ret = rI->getReturnValue();
                auto tmp = I;
                I--;
                tmp->eraseFromParent();
              }
              else if (i>1){
                // add autda intrinsic 
                auto autda_instr = CAuthIntr::autda(F, *I, canary_val);
                canary_val = Builder.CreateLoad(autda_instr);
              }
            }
          }
        }
        
        if (BB.getName()=="TrueBB"){
          // insert original return instruction to execute when canaries remain intact
          llvm::ReturnInst::Create(C, save_ret, TrueBB);
        }
        else if (BB.getName()=="FalseBB"){
          CAuthIR::CreateFailBB(C, &F, FalseBB, save_ret);
        }
      }
      return true; 
    }
  };
}

char CAuthIR::ID = 0;
static RegisterPass<CAuthIR> X("cauth-ir-standalone", "CAuth IR standalone pass");

BasicBlock* CAuthIR::CreateEmptyBB(LLVMContext &C, const Twine &Name, Function *Parent, BasicBlock *InsertBefore){
  return llvm::BasicBlock::Create(C, Name, Parent, InsertBefore);
}

//Inserts canary_chk_fail instructions into the FalseBB
void CAuthIR::CreateFailBB(LLVMContext &C, Function *F, BasicBlock *FalseBB, Value *save_ret){
  IRBuilder<> B(FalseBB);
  Module* M = F->getParent();
  auto arg = B.CreateGlobalString("\n***Canary Check Failed***\nExiting....\n\n", "__canary_chk_fail");
  // print "canary check failed" message
  Constant *printfFunc = M->getOrInsertFunction("printf", FunctionType::get(IntegerType::getInt32Ty(C), 
                        PointerType::get(Type::getInt8Ty(C), 0)) );
  B.CreateCall(printfFunc, {arg}, "printfCall");
  Value *one = ConstantInt::get(Type::getInt32Ty(M->getContext()),1);
  FunctionType *fType = FunctionType::get(Type::getVoidTy(C), Type::getInt32Ty(C), false);
  // exit
  Constant *exitF = M->getOrInsertFunction("exit", fType);
  B.CreateCall(exitF,one);
  llvm::ReturnInst::Create(C, save_ret, FalseBB);
}