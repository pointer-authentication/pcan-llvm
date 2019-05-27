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
//#include "llvm/CAUTH/Cauth.h"
#include "CauthUtils.h"

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
   //void convertCauthIntrinsic(MachineBasicBlock &MBB, MachineInstr &MI, unsigned instr, bool isMod);

 private:
    const TargetMachine *TM = nullptr;
    const AArch64Subtarget *STI = nullptr;
    const AArch64InstrInfo *TII = nullptr;
    const AArch64RegisterInfo *TRI = nullptr;

    CauthUtils_ptr cauthUtils_ptr = nullptr;

    uint16_t funcID=0;

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
  ++funcID;
  unsigned ispacga=0;
  unsigned ispacda=0;
  unsigned isautda=0;
  TM = &MF.getTarget();;
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();
  cauthUtils_ptr = CauthUtils::get(TRI, TII);

  //Calling convention is default C (AAPCS) calling convention.

  for (auto &MBB : MF) {
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end(); MIi++) {
      const auto MIOpcode = MIi->getOpcode();
      switch(MIOpcode) {
        default:
          break;
        case AArch64::CAUTH_PACGA:
        { 
          ++ispacga;
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACGA, funcID, ispacga, ispacda, isautda);
          found = true; 
          break;
        }
        
        case AArch64::CAUTH_PACDA:
        {
          ++ispacda;
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACDA, funcID, ispacga, ispacda, isautda);
          break;
        }
        case AArch64::CAUTH_AUTDA:
        {
          ++isautda;  
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::AUTDA, funcID, ispacga, ispacda, isautda);
          found = true; // make sure we return true when we modify stuff

          break;
        }
      }
    }
      //MBB.dump();
  }

  return found;
}



