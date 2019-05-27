//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/CAUTH/CauthIntr.h>
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
using namespace CAUTH;

#define DEBUG_TYPE "cauth-ir-all:\t"

STATISTIC(TotalFunctionCounter, "Total number of functions in code");
STATISTIC(FunctionCounter, "number of functions instrumented");
STATISTIC(VarCounter, "Total number of local variables instrumented");


namespace {
  // CAuthIR - 
  struct CAuthIR : public FunctionPass {
    
  public:
    static char ID; // Pass identification
    long long funcID = 0ll; 

    CAuthIR() : FunctionPass(ID) {}

    BasicBlock* CreateEmptyBB(LLVMContext &C, const Twine &Name="", 
                              Function *Parent=nullptr, BasicBlock *InsertBefore=nullptr );

    void CreateFailBB(LLVMContext &C, Function *F, BasicBlock *FalseBB, Value *save_ret);

    bool runOnFunction(Function &F) override {
      ++TotalFunctionCounter;
      ++funcID;
      unsigned numBuffs = 0;
      Value* oldcbuff = nullptr;
      Instruction *loc = nullptr;
      auto &C = F.getParent()->getContext();
      BasicBlock* TrueBB=nullptr;
      BasicBlock* FalseBB=nullptr;
      Type* buffTy = nullptr;
      Value *pacga_instr = nullptr;
      Value *pacda_instr = nullptr;
      Value *save_ret = nullptr;
      AllocaInst* arr_alloc = nullptr;
      for (auto &BB : F){
        for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I){
          if(isa<AllocaInst>(*I) && BB.getName()=="entry"){
              ++VarCounter;
              loc = &*I;
              IRBuilder<> Builder(loc);

              unsigned i = 0;
              Type* tmp = nullptr;
              while (i <= numBuffs){
                if (i==0){
                  buffTy = Type::getInt64Ty(C);
                }else{
                  tmp = PointerType::get(buffTy, 0);
                  buffTy = tmp;
                }
                i++;
              }

              arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "cauth_alloc");
              ++numBuffs;
              
              if (numBuffs==1){
                ++FunctionCounter;
                pacga_instr = CauthIntr::pacga(F, *loc, funcID);
                oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
                Builder.CreateAlignedStore(pacga_instr, arr_alloc, 8);
              }
              else if (numBuffs>1){
                pacda_instr = CauthIntr::pacda(F, *loc, oldcbuff);
                oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
                Builder.CreateAlignedStore(pacda_instr, oldcbuff, 8);
              }
          }
          else if(isa<ReturnInst>(I) && numBuffs>0){
            Instruction *inst= &*I;
            IRBuilder<> Builder(inst);
            llvm::ReturnInst* rI = dyn_cast<llvm::ReturnInst>(inst);
            auto canary_val = Builder.CreateLoad(oldcbuff);
            for (int i=numBuffs; i>0; i--){
              if (i == 1){
                auto pacga2_instr = CauthIntr::pacga(F, *I, funcID);
                auto cmp = Builder.CreateICmp(llvm::CmpInst::ICMP_EQ, canary_val, pacga2_instr, "cmp");
                TrueBB= CAuthIR::CreateEmptyBB(C, "TrueBB", &F);
                FalseBB= CAuthIR::CreateEmptyBB(C, "FalseBB", &F);
                Builder.CreateCondBr(cmp, TrueBB, FalseBB);
                save_ret = rI->getReturnValue();
                auto tmp = I;
                I--;
                tmp->eraseFromParent();
              }
              else if (i>1){
              Value* autda_instr = CauthIntr::autda(F, *I, canary_val);
              canary_val = Builder.CreateLoad(autda_instr);
              }
            }
          }         
        }        
         if (BB.getName()=="TrueBB"){
            llvm::ReturnInst::Create(C, save_ret, TrueBB);
          }else if (BB.getName()=="FalseBB"){
            CAuthIR::CreateFailBB(C, &F, FalseBB, save_ret);
          }          
        }
      return true; 
    }
  };
}

char CAuthIR::ID = 0;
static RegisterPass<CAuthIR> X("cauth-ir-all", "CAuth IR pass for all local variables");

BasicBlock* CAuthIR::CreateEmptyBB(LLVMContext &C, const Twine &Name, Function *Parent, BasicBlock *InsertBefore){
  return llvm::BasicBlock::Create(C, Name, Parent, InsertBefore);
}

//Inserts canary_chk_fail instructions into the FalseBB
void CAuthIR::CreateFailBB(LLVMContext &C, Function *F, BasicBlock *FalseBB, Value *save_ret){
  IRBuilder<> B(FalseBB);
  Module* M = F->getParent();
  auto arg = B.CreateGlobalString("\n***Canary Check Failed***\nExiting....\n\n", "__canary_chk_fail");
  Constant *printfFunc = M->getOrInsertFunction("printf", FunctionType::get(IntegerType::getInt32Ty(C), 
                        PointerType::get(Type::getInt8Ty(C), 0)) );
  B.CreateCall(printfFunc, {arg}, "printfCall");
  Value *one = ConstantInt::get(Type::getInt32Ty(M->getContext()),1);
  FunctionType *fType = FunctionType::get(Type::getVoidTy(C), Type::getInt32Ty(C), false);
  Constant *exitF = M->getOrInsertFunction("exit", fType);
  B.CreateCall(exitF,one);

  llvm::ReturnInst::Create(C, save_ret, FalseBB);

}