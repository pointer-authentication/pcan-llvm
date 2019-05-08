//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/IR/Constants.h>
#include "CauthUtils.h"

using namespace llvm;
using namespace llvm::CAUTH;

CauthUtils::CauthUtils(const TargetRegisterInfo *TRI, const TargetInstrInfo *TII) :
    TII(TII),
    TRI(TRI)
{};

void CauthUtils::convertCauthIntrinsic(MachineBasicBlock &MBB, MachineInstr &MI, unsigned instr, bool hasMod, unsigned funcID) {
  const auto &DL = MI.getDebugLoc();
  const unsigned dst = MI.getOperand(0).getReg();
  const unsigned src = MI.getOperand(1).getReg();
  unsigned mod = AArch64::X9; //MI.getOperand(2).getReg();		//modifier register
  /*if (MI.getOperand(2).isKill()) {
        unsigned oldMod = mod;
        mod = AArch64::X26;
        BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), mod).addReg(oldMod).addImm(0).addImm(0);
  }*/
 
  if (instr==AArch64::PACGA){
    BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), mod).addReg(AArch64::SP).addImm(0).addImm(0);
    BuildMI(MBB, MI, DL, TII->get(AArch64::MOVKXi), mod).addReg(mod).addImm(funcID).addImm(48);
  	if (!CAUTH::useDummy()){
      BuildMI(MBB, MI, DL, TII->get(AArch64::PACGA), dst).addReg(src).addReg(mod);
  	}
  	else{
  	  insertPAInstr(MBB, &MI, dst, mod, TII->get(instr), DL, hasMod);
  	}
  }
  else if (instr==AArch64::PACDA || instr==AArch64::AUTDA){
    BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), mod).addReg(AArch64::SP).addImm(0).addImm(0);
    BuildMI(MBB, MI, DL, TII->get(AArch64::MOVKXi), mod).addReg(mod).addImm(funcID).addImm(48);
    if (instr==AArch64::PACDA){
      BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), dst).addReg(src).addImm(0).addImm(0);
    }
    insertPAInstr(MBB, &MI, dst, mod, TII->get(instr), DL, hasMod);
  }
  
  // And finally, remove the intrinsic
  MI.removeFromParent();
}

void CauthUtils::insertPAInstr(MachineBasicBlock &MBB, MachineBasicBlock::instr_iterator MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL, bool hasMod) {
  insertPAInstr(MBB, (MBB.instr_end() == MIi ? nullptr : &*MIi), ptrReg, modReg, MCID, DL, hasMod);
}

void CauthUtils::insertPAInstr(MachineBasicBlock &MBB, MachineInstr *MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL, bool hasMod) {
    if (!CAUTH::useDummy()){
      if (MIi == nullptr) {
        BuildMI(&MBB, DL, MCID).addReg(ptrReg).addReg(modReg);
      } else {
      	if (hasMod){
          BuildMI(MBB, MIi, DL, MCID, ptrReg).addReg(modReg);
    	}
    	else{
    	  BuildMI(MBB, MIi, DL, MCID, ptrReg);
    	}
      }
    }
    else{
      addNops(MBB, MIi, ptrReg, modReg, DL, hasMod);
    }
}

void CauthUtils::addNops(MachineBasicBlock &MBB, MachineInstr *MI, unsigned ptrReg, unsigned modReg, const DebugLoc &DL, bool hasMod) {
  if (MI == nullptr) {
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(17);
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(37);
    BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(97);
    if (hasMod){
      /*BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), AArch64::X24).addReg(modReg).addImm(0).addImm(0);
      BuildMI(&MBB, DL, TII->get(AArch64::EORXrs)).addReg(ptrReg).addReg(ptrReg).addReg(AArch64::X24).addImm(0);*/
      
      BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(117); 
    }
	else{
	  BuildMI(&MBB, DL, TII->get(AArch64::EORXri)).addReg(ptrReg).addReg(ptrReg).addImm(117);	
	  }
  } else {
    BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri), ptrReg).addReg(ptrReg).addImm(17);
    BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri), ptrReg).addReg(ptrReg).addImm(37);
    BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri), ptrReg).addReg(ptrReg).addImm(97);
    if (hasMod){
      BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri), ptrReg).addReg(ptrReg).addImm(117);
      /*BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), AArch64::X24).addReg(modReg).addImm(0).addImm(0);
      BuildMI(&MBB, DL, TII->get(AArch64::EORXrs)).addReg(ptrReg).addReg(ptrReg).addReg(AArch64::X24).addImm(0);*/
  	}
	else{
	  BuildMI(MBB, MI, DL, TII->get(AArch64::EORXri), ptrReg).addReg(ptrReg).addImm(117);
	  }
  }
}

