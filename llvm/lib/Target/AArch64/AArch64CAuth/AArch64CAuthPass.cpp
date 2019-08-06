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

namespace {
 class AArch64CAuthPass : public MachineFunctionPass {

 public:
   static char ID;

   AArch64CAuthPass() : MachineFunctionPass(ID) {}

   StringRef getPassName() const override { return DEBUG_TYPE; }

   bool doInitialization(Module &M) override;
   bool runOnMachineFunction(MachineFunction &) override;

 private:
   const AArch64Subtarget *STI = nullptr;
   const AArch64InstrInfo *TII = nullptr;
   const AArch64RegisterInfo *TRI = nullptr;

   inline void convertMod(MachineBasicBlock &MBB, MachineInstr &MI,
                          uint16_t funcID);
   inline void convertPacga(MachineBasicBlock &MBB, MachineInstr &MI);
   inline void convertPaIntr(MachineBasicBlock &MBB, MachineInstr &MI,
                             unsigned opcode);

   static inline uint16_t getFuncID(const Function &F);
 };
} // end anonymous namespace

FunctionPass *llvm::createCauthPass() {
  return new AArch64CAuthPass();
}

char AArch64CAuthPass::ID = 0;

void AArch64CAuthPass::convertMod(MachineBasicBlock &MBB,
                                  MachineInstr &MI,
                                  uint16_t funcID) {
  const unsigned reg = MI.getOperand(0).getReg();
  const auto &DL = MI.getDebugLoc();
  BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), reg)
      .addReg(AArch64::SP)
      .addImm(0)
      .addImm(0);
  BuildMI(MBB, MI, DL, TII->get(AArch64::MOVKXi), reg)
      .addReg(reg)
      .addImm(funcID)
      .addImm(48);
  MI.removeFromParent();
}

void AArch64CAuthPass::convertPacga(MachineBasicBlock &MBB,
                                  MachineInstr &MI) {
  const unsigned dst = MI.getOperand(0).getReg();
  const unsigned mod = MI.getOperand(1).getReg();
  const auto &DL = MI.getDebugLoc();

  BuildMI(MBB, MI, DL, TII->get(AArch64::PACGA))
      .addDef(dst)
      .addUse(mod)
      .addUse(mod);

  MI.removeFromParent();
}

void AArch64CAuthPass::convertPaIntr(MachineBasicBlock &MBB, MachineInstr &MI,
                                     const unsigned opcode) {
  assert(opcode == AArch64::AUTDA || opcode == AArch64::PACDA);

  const unsigned dst = MI.getOperand(0).getReg();
  const unsigned mod = MI.getOperand(2).getReg();
  const auto &DL = MI.getDebugLoc();

  BuildMI(MBB, MI, DL, TII->get(opcode))
      .addDef(dst)
      .addUse(mod);

  MI.removeFromParent();
}

inline uint16_t AArch64CAuthPass::getFuncID(const Function &F) {
  uint16_t funcID;
  std::istringstream iss(F.getFnAttribute("cauth-funcid")
                             .getValueAsString());
  iss >> funcID;
  return funcID;
}

bool AArch64CAuthPass::doInitialization(Module &M) {
  return true;
}

bool AArch64CAuthPass::runOnMachineFunction(MachineFunction &MF) {
  bool found = false;
  const auto &F = MF.getFunction();

  if (!F.hasFnAttribute("cauth-funcid"))
    return false;

  auto funcID = getFuncID(F);

  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();

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
          convertMod(MBB, MI, funcID);
          break;
        case AArch64::CAUTH_PACGA:
          convertPacga(MBB, MI);
          found = true;
          break;
        case AArch64::CAUTH_PACDA:
          convertPaIntr(MBB, MI, AArch64::PACDA);
          break;
        case AArch64::CAUTH_AUTDA:
          convertPaIntr(MBB, MI, AArch64::AUTDA);
          break;
      }
    }
  }

  return found;
}



