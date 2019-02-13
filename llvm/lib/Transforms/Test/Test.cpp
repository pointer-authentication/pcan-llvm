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
using namespace llvm;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");

namespace {
  struct Test : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Test() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      Instruction *loc = nullptr;
      auto &C = F.getParent()->getContext();  //Get current context
      AllocaInst* arr_alloc = nullptr;
      errs().write_escaped(F.getName()) << '\n';  //Print function name
      for (auto &BB : F){
        errs().write_escaped(BB.getName()) << '\n';  //Print Basic block's name
        for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I){
          I->dump();  //Dump instruction
          if(isa<AllocaInst>(*I))  //Check if an alloca instruction is encountered
          {  
            if(I->getName().find("test_alloc") == std::string::npos && I->getName() != "retval"){
              loc = I->getNextNode();  //Get instruction location
              IRBuilder<> Builder(loc);  //Create a builder
              auto buffTy = Type::getInt64Ty(C);
              arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "test_alloc");  //Insert alloca instruction with name test_alloc
            }
          }
        }
      }
      return false;
    }
  };
}

char Test::ID = 0;
static RegisterPass<Test> X("test-ir", "Test Pass");
