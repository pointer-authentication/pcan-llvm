//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "CAuthUtils.h"
#include "llvm/IR/Constants.h"

using namespace llvm;
using namespace llvm::CAuth;

CAuthUtils::CAuthUtils(const TargetRegisterInfo *TRI, const TargetInstrInfo *TII) :
    TII(TII),
    TRI(TRI)
{};

void CAuthUtils::convertCauthIntrinsic(MachineBasicBlock &MBB, MachineInstr &MI, unsigned instr,
                                              unsigned funcID, unsigned ispacga, unsigned ispacda, unsigned isautda) {
  const auto &DL = MI.getDebugLoc();
  const unsigned dst = MI.getOperand(0).getReg();
  const unsigned src = MI.getOperand(1).getReg();
  unsigned mod = AArch64::X9; 
  // ensure that modifier is generated only once for prologue and epilogue each
  if ((ispacga==1 && ispacda==0) || (ispacda==1 && ispacga==0) || isautda==1 || (ispacga==2 && isautda==0) ) {
    BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), mod).addReg(AArch64::SP).addImm(0).addImm(0);
    BuildMI(MBB, MI, DL, TII->get(AArch64::MOVKXi), mod).addReg(mod).addImm(funcID).addImm(48);
  }
  if (instr==AArch64::PACGA){
  	if (!CAuth::useDummy()){
      BuildMI(MBB, MI, DL, TII->get(AArch64::PACGA), dst).addReg(src).addReg(mod);
  	}
  	else{
  	  insertPAInstr(MBB, &MI, dst, mod, TII->get(instr), DL);
  	}
  }
  else if (instr==AArch64::PACDA || instr==AArch64::AUTDA){
    insertPAInstr(MBB, &MI, dst, mod, TII->get(instr), DL);
  }
  
  // And finally, remove the intrinsic
  MI.removeFromParent();
}

void CAuthUtils::insertPAInstr(MachineBasicBlock &MBB, MachineBasicBlock::instr_iterator MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL) {
  insertPAInstr(MBB, (MBB.instr_end() == MIi ? nullptr : &*MIi), ptrReg, modReg, MCID, DL);
}

void CAuthUtils::insertPAInstr(MachineBasicBlock &MBB, MachineInstr *MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL) {
    if (!CAuth::useDummy()){
      if (MIi == nullptr) {
        BuildMI(&MBB, DL, MCID).addReg(ptrReg).addReg(modReg);
      } 
      else {
        BuildMI(MBB, MIi, DL, MCID, ptrReg).addReg(modReg);
      }
    }
    else{
      addNops(MBB, MIi, ptrReg, modReg, DL);
    }
}

void CAuthUtils::addNops(MachineBasicBlock &MBB, MachineInstr *MI, unsigned ptrReg, unsigned modReg, const DebugLoc &DL) {
  if (MI == nullptr){
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(17);
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(37);
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(97);
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(117);	
  }
}

