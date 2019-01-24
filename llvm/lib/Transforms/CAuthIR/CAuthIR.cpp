//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


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
      for (auto &BB : F){
        for (auto &I : BB){
          errs() << DEBUG_TYPE;
          
          if(isa<AllocaInst>(I)){
            llvm::AllocaInst* aI = dyn_cast<llvm::AllocaInst>(&I);
            auto* arr_ty = aI->getAllocatedType(); 
            //errs()<<arr_ty->getTypeID();
            if(I.getName().find("cauth_buff") == std::string::npos){
              Type* intType = IntegerType::getInt64Ty(F.getParent()->getContext());
              auto num = 1;
              ArrayType* arrayType = ArrayType::get(intType, num);
              Instruction *loc = I.getNextNode();
              AllocaInst* arr_alloc = new AllocaInst(arrayType , 0, "cauth_buff", loc);
              //I.dump();
              //loc->dump();
            }
          }
        }
         BB.dump();
      }
      return true; //InsertStackProtectors();
    }
  };
}

char CAuthIR::ID = 0;
static RegisterPass<CAuthIR> X("cauth-ir", "CAuth IR Pass");

