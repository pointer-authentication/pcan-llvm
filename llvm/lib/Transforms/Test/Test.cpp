//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include <llvm/CAUTH/CauthIntr.h>

using namespace llvm;
using namespace CAUTH;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");

namespace {
  struct Test : public FunctionPass {
    // Pass identification, replacement for typeid
    static char ID;
    Test() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      //Get current context
      auto &C = F.getParent()->getContext();  
      errs()<<"Function Name: ";
      errs().write_escaped(F.getName()) << '\n';  //Print function name
      for (auto &BB : F){
        errs()<<"Basic Block: ";
        errs().write_escaped(BB.getName()) << '\n';  //Print Basic block's name
        for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I){
          //I->dump();  //Dump instruction
          //Check if an alloca instruction is encountered
          if(isa<AllocaInst>(*I))  
          {   //Get instruction location
              llvm::AllocaInst *aI = dyn_cast<llvm::AllocaInst>(&*I);
              if (aI->getAllocatedType()->isArrayTy())
              {
              Instruction *loc = I->getNextNode();  
               //Create a builder
              IRBuilder<> Builder(loc); 
              auto buffTy = Type::getInt64Ty(C);
              //Insert alloca instruction with name test_alloc
              AllocaInst* arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "test_alloc");  
              ++I;
            }
          }
        }
       // BB.dump();
      }
      return false;
    }
  };
}

char Test::ID = 0;
static RegisterPass<Test> X("test-ir", "Test Pass");
