//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
// LLVM includes
#include "AArch64.h"
#include "AArch64Subtarget.h"
#include "AArch64RegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
// CAUTH includes
#include "llvm/CAUTH/Cauth.h"

#define DEBUG_TYPE "aarch64-test"

using namespace llvm;
using namespace llvm::CAUTH;


namespace {
 class TestPass : public MachineFunctionPass {

 public:
   static char ID;
   TestPass() :
   MachineFunctionPass(ID) {}
   //log(PARTS::PartsLog::getLogger(DEBUG_TYPE)){}

   StringRef getPassName() const override { return DEBUG_TYPE; }

   bool doInitialization(Module &M) override;
   bool runOnMachineFunction(MachineFunction &) override;
 };
} // end anonymous namespace

FunctionPass *llvm::createTestPass() {
  return new TestPass();
}

char TestPass::ID = 0;

bool TestPass::doInitialization(Module &M) {
  return true;
}

bool TestPass::runOnMachineFunction(MachineFunction &MF) {
  
  for (auto &MBB : MF) {
    errs()<<MF.getName()<<"\n"<< MBB.getName() << "\n";
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end(); MIi++) {
      //errs()<< MBB.getName() << "\n";
      MIi->dump();
    }
     // MBB.dump();
  }

  return true;
}


