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

#define DEBUG_TYPE "\ncauth-ir:\t"



//FunctionPass *llvm::createCAuthIRPass() { return new CAuthIR(); }

namespace {
  // CAuthIR - 
  struct CAuthIR : public FunctionPass {
    
  public:
    static char ID; // Pass identification
  
    CAuthIR() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << DEBUG_TYPE;
      errs().write_escaped(F.getName()) << '\n';
      unsigned numBuffs = 0;
      Value* oldcbuff = nullptr;
      Instruction *loc = nullptr;
      auto &C = F.getParent()->getContext();
      Type* buffTy = nullptr;
      //Type* int64PtrTy = PointerType::get(int64Ty, 0);
      Value *pacga_instr = nullptr;
      Value *pacda_instr = nullptr;
      AllocaInst* arr_alloc = nullptr;
      for (auto &BB : F){
        for (auto &I : BB){
          //errs() << DEBUG_TYPE;
          if(isa<AllocaInst>(I)){
            llvm::AllocaInst* aI = dyn_cast<llvm::AllocaInst>(&I);
            if(I.getName().find("cauth_alloc") == std::string::npos && I.getName().find("retval") == std::string::npos){
              auto num = 1;
              loc = I.getNextNode();
              IRBuilder<> Builder(loc);

              int i = 0;
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
                pacga_instr = CauthIntr::pacga(F, *loc);
                Builder.CreateAlignedStore(pacga_instr, arr_alloc, 8);
                oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
              }
              else if (numBuffs>1){
                pacda_instr = CauthIntr::pacda(F, *loc, oldcbuff);
                oldcbuff = llvm::cast<llvm::Value>(arr_alloc);
                Builder.CreateAlignedStore(pacda_instr, oldcbuff, 8);
              }
            }
          }
          else if(isa<ReturnInst>(I) && numBuffs>0){
            IRBuilder<> Builder(&I);
            auto canary_val = Builder.CreateLoad(oldcbuff);
            for (int i=numBuffs; i>0; i--){
              if (i == 1){
                auto pacga2_instr = CauthIntr::pacga(F, I);
                auto cmp = new ICmpInst(&I, llvm::CmpInst::ICMP_EQ, canary_val, pacga2_instr);
              }
              else if (i>1){
              Value* autda_instr = CauthIntr::autda(F, I, canary_val);
              canary_val = Builder.CreateLoad(autda_instr);
              }
            }
          }
        }
         BB.dump();
      }
      return true; 
    }
  };
}

char CAuthIR::ID = 0;
static RegisterPass<CAuthIR> X("cauth-ir", "CAuth IR Pass");

