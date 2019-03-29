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
  TM = &MF.getTarget();;
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  TRI = STI->getRegisterInfo();
  cauthUtils_ptr = CauthUtils::get(TRI, TII);

  for (auto &MBB : MF) {
    //errs()<<MF.getName()<<"\n"<< MBB.getName() << "\n";
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end(); MIi++) {
      //MIi->dump();
      const auto MIOpcode = MIi->getOpcode();
      //errs()<<"Opcode:\t"<<MIOpcode<<"\n";

      switch(MIOpcode) {
        default:
          break;
        case AArch64::CAUTH_PACGZA:
        { 
          //errs()<<"\nInside CAUTH_PACGA Case\n";
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACGA, true);
          found = true; 
          break;
        }
        
        case AArch64::CAUTH_PACDA:
        {
          //errs()<<"\nInside CAUTH_PACDA Case\n";
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::PACDA, true);
          break;
        }
        case AArch64::CAUTH_AUTDA:
        {
          //errs()<<"\nInside CAUTH_AUTDA Case\n";
            
          auto &MI = *MIi--;
          cauthUtils_ptr->convertCauthIntrinsic(MBB, MI, AArch64::AUTDA, true);
          found = true; // make sure we return true when we modify stuff

          break;
        }
      }
    }
     // MBB.dump();
  }

  return found;
}


/*void CauthPass::convertCauthIntrinsic(MachineBasicBlock &MBB, MachineInstr &MI, unsigned instr) {
  const auto &DL = MI.getDebugLoc();
  const unsigned dst = MI.getOperand(0).getReg();
  const unsigned src = MI.getOperand(1).getReg();
  //unsigned mod = AArch64::SP;
  
  if (instr==AArch64::PACGA){
    unsigned mod = AArch64::SP;
    //unsigned mod = MI.getOperand(2).getReg();
    BuildMI(MBB, MI, DL, TII->get(AArch64::PACGA), dst).addReg(src).addReg(mod);
  }
  else if (instr==AArch64::PACDZA || instr==AArch64::AUTDZA){
    //unsigned mod = MI.getOperand(2).getReg();
    // Save the mod register if it is marked as killable!
    // Move the pointer to destination register
    //BuildMI(MBB, MI, DL, TII->get(AArch64::ADDXri), dst).addReg(src).addImm(0).addImm(0);
    insertPAInstr(MBB, &MI, dst, 0, TII->get(instr), DL);
  }
  // And finally, remove the intrinsic
  MI.removeFromParent();
}





void CauthPass::insertPAInstr(MachineBasicBlock &MBB, MachineBasicBlock::instr_iterator MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL) {
  insertPAInstr(MBB, (MBB.instr_end() == MIi ? nullptr : &*MIi), ptrReg, modReg, MCID, DL);
}

void CauthPass::insertPAInstr(MachineBasicBlock &MBB, MachineInstr *MIi, unsigned ptrReg,
                               unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL) {
    if (MIi == nullptr){
      BuildMI(&MBB, DL, MCID).addReg(ptrReg);
    } else {
      BuildMI(MBB, MIi, DL, MCID, ptrReg);
    }
}*/

