//===----------------------------------------------------------------------===//
//
// Author: Zaheer Gauhar <zaheer.gauhar@pm.me>
// *******Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/**********
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

#define DEBUG_TYPE "aarch64-cauth"

using namespace llvm;
using namespace llvm::CAUTH;


namespace {
 class CauthPass : public MachineFunctionPass {

 public:
   static char ID;
   CauthPass() :
   MachineFunctionPass(ID) {}
   //log(PARTS::PartsLog::getLogger(DEBUG_TYPE)){}

   StringRef getPassName() const override { return DEBUG_TYPE; }

   bool doInitialization(Module &M) override;
   bool runOnMachineFunction(MachineFunction &) override;
   bool instrumentBranches(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::instr_iterator &MIi);

 private:
   
 };
} // end anonymous namespace

FunctionPass *llvm::createCauthPass() {
  return new CauthPass();
}

char CauthPass::ID = 0;

bool CauthPass::doInitialization(Module &M) {
  return true;
}

bool CauthPass::runOnMachineFunction(MachineFunction &MF) {
  DEBUG(dbgs() << getPassName() << ", function " << MF.getName() << '\n');
  //DEBUG("function " << MF.getName() << "\n");
  errs()<< getPassName() << ", function " << MF.getName() << '\n';
  for (auto &MBB : MF){
    errs()<< getPassName() << ", function " << MF.getName() << ", Basic Block: " << MBB.getName() << '\n';
    for (auto &I : MBB){
      I.dump();
    }
  }
  return true;
}
