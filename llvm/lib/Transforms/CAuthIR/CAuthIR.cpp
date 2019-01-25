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

#define DEBUG_TYPE "cauth-ir"



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
      for (auto &BB : F){
        for (auto &I : BB){
          errs() << DEBUG_TYPE;
          
          if(isa<AllocaInst>(I)){
            llvm::AllocaInst* aI = dyn_cast<llvm::AllocaInst>(&I);
            
            if(I.getName().find("cauth_alloc") == std::string::npos){
               auto &C = F.getParent()->getContext();
              Type* int64Ty = Type::getInt64Ty(C);
              auto num = 1;
              ArrayType* arrayType = ArrayType::get(int64Ty, num);
              loc = I.getNextNode();
              AllocaInst* arr_alloc = new AllocaInst(int64Ty , 0, "cauth_alloc", loc);
              oldcbuff = arr_alloc;
              ++numBuffs;
              Value* pac_instr = nullptr;
              if (numBuffs==1)
                pac_instr = CauthIntr::pacga(F, *loc);
              else if (numBuffs>1)
                pac_instr = CauthIntr::pacda(F, *loc, oldcbuff);
              //auto store = new StoreInst(paced_instr, arr_alloc, loc);
            }
          }
          else if(isa<ReturnInst>(I)){
            auto aut_instr = CauthIntr::autda(F, I, oldcbuff);
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

