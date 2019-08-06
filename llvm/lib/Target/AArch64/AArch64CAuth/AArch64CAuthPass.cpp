//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// LLVM includes
#include "CAuthUtils.h"
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
#include <iostream>
#include <sstream>

#define DEBUG_TYPE "aarch64-cauth"

using namespace llvm;
using namespace llvm::CAuth;


namespace {
 class AArch64CAuthPass : public MachineFunctionPass {

 public:
   static char ID;
   AArch64CAuthPass() :
   MachineFunctionPass(ID) {}
   StringRef getPassName() const override { return DEBUG_TYPE; }

   bool doInitialization(Module &M) override;
   bool runOnMachineFunction(MachineFunction &) override;

 private:
   const AArch64Subtarget *STI = nullptr;
   const AArch64InstrInfo *TII = nullptr;
   const AArch64RegisterInfo *TRI = nullptr;

   CauthUtils_ptr cauthUtils_ptr = nullptr;

   void convertModIntrinsic(MachineBasicBlock &MBB,
                            MachineInstr &MI,
                            uint16_t funcID) {
     const unsigned reg = MI.getOperand(0).getReg();
     const auto &DL = MI.getDebugLoc();
     BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), reg)
         .addReg(AArch64::SP)
         .addImm(0)
         .addImm(0);
     auto ret = BuildMI(MBB, MI, DL, TII->get(AArch64::MOVKXi), reg)
         .addReg(reg)
         .addImm(funcID)
         .addImm(48);
     MI.removeFromParent();
   }

   static inline uint16_t getFuncID(const Function &F) {
      uint16_t funcID;
      std::istringstream iss(F.getFnAttribute("cauth-funcid").getValueAsString());
      iss >> funcID;
      return funcID;
    }
 };
} // end anonymous namespace

FunctionPass *llvm::createCauthPass() {
  return new AArch64CAuthPass();
}

char AArch64CAuthPass::ID = 0;

bool AArch64CAuthPass::doInitialization(Module &M) {
  return true;
}

bool AArch64CAuthPass::runOnMachineFunction(MachineFunction &MF) {
  bool found = false;
  const auto &F = MF.getFunction();

  if (!F.hasFnAttribute("cauth-funcid"))
    return false;

  auto funcID = getFuncID(F);

  unsigned ispacga=0;
  unsigned ispacda=0;
  unsigned isautda=0;
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();
  cauthUtils_ptr = CAuthUtils::get(TRI, TII);

  //Calling convention is default C (AAPCS) calling convention.

  for (auto &MBB : MF) {
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end();) {
      auto &MI = *MIi;
      ++MIi;

      switch(MI.getOpcode()) {
        default:
          break;
        case AArch64::CAUTH_PRO_MOD:
          LLVM_FALLTHROUGH;
        case AArch64::CAUTH_EPI_MOD:
          convertModIntrinsic(MBB, MI, funcID);
          break;
        case AArch64::CAUTH_PACGA:
        { 
          ++ispacga;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACGA, funcID, ispacga, ispacda, isautda);
          found = true; 
          break;
        }
        
        case AArch64::CAUTH_PACDA:
        {
          ++ispacda;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACDA, funcID, ispacga, ispacda, isautda);
          found = true; // make sure we return true when we modify stuff
          break;
        }
        case AArch64::CAUTH_AUTDA:
        {
          ++isautda;  
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::AUTDA, funcID, ispacga, ispacda, isautda);
          found = true; // make sure we return true when we modify stuff
          break;
        }
      }
    }
  }
  return found;
}



