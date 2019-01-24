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
   const AArch64Subtarget *STI = nullptr;
   const AArch64InstrInfo *TII = nullptr;
   const AArch64RegisterInfo *TRI = nullptr;
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
  bool found = false;
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  //TM = &MF.getTarget();;
  TRI = STI->getRegisterInfo();

  for (auto &MBB : MF) {
    errs()<<MF.getName()<<":--->\n"<< MBB.getName() << ":--->\n";
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end(); MIi++) {
      //errs()<< MBB.getName() << "\n";
      MIi->dump();
      const auto MIOpcode = MIi->getOpcode();
      switch(MIOpcode) {
        default:
          break;
        case AArch64::CAUTH_PACGA:
          break;
        case AArch64::CAUTH_PACDA:
        case AArch64::CAUTH_AUTDA:
          const auto &DL = MIi->getDebugLoc();
          const unsigned dst = MIi->getOperand(0).getReg();
          const unsigned src = MIi->getOperand(1).getReg();
          unsigned mod = MIi->getOperand(2).getReg();

          // Save the mod register if it is marked as killable!
          if (MIi->getOperand(2).isKill()) {
            unsigned oldMod = mod;
            mod = AArch64::X24;
            BuildMI(MBB, MIi, DL, TII->get(AArch64::ADDXri), mod).addReg(oldMod).addImm(0).addImm(0);
          }
          // Move the pointer to destination register
          BuildMI(MBB, MIi, DL, TII->get(AArch64::ADDXri), dst).addReg(src).addImm(0).addImm(0);

          // Insert appropriate PA instruction
          /*
          if (MIOpcode == AArch64::CAUTH_PACDA) {
            log->inc(TAG ".pacia", true) << "converting CAUTH_PACDA\n";
            partsUtils->insertPAInstr(MBB, MIi, dst, mod, TII->get(AArch64::PACIA), DL);
            partsUtils->addEventCallFunction(MBB, *MIi, DL, funcCountCodePtrCreate);
          } else if (MIOpcode == AArch64::CAUTH_AUTDA) {
            //log->inc(TAG ".pacda", true) << "converting CAUTH_AUTDA\n";
            partsUtils->insertPAInstr(MBB, MIi, dst, mod, TII->get(AArch64::PACDA), DL);
            partsUtils->addEventCallFunction(MBB, *MIi, DL, funcCountDataStr);
          }
          */
          // And finally, remove the intrinsic
          auto tmp = MIi;
          MIi--;
          tmp->removeFromParent();

          found = true; // make sure we return true when we modify stuff

          break;
      }

    }
  }

  return found;
}
