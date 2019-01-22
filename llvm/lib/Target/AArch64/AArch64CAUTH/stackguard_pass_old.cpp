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
   const AArch64Subtarget *STI = nullptr;
   const AArch64InstrInfo *TII = nullptr;
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
  STI = &MF.getSubtarget<AArch64Subtarget>();
  TII = STI->getInstrInfo();
  
  for (auto &MBB : MF){
    errs()<< getPassName() << ", function " << MF.getName() << "\nBasic Block: " << MBB.getName() << '\n';
    bool isPrologue = true;
    auto src = 0;
    for (auto MIi = MBB.instr_begin(); MIi != MBB.instr_end(); MIi++) {
      //errs()<<'\t';
      //MIi->dump();
      if (MBB.getName()=="entry"){
        const auto MIOpcode = MIi->getOpcode();
        std::string type_str;
        llvm::raw_string_ostream rso(type_str);
        auto dst1 = AArch64::X24;
        auto dst2 = AArch64::X25;
        switch(MIOpcode) {
          default:
            break;
          //case AArch64::ADRP:
          case AArch64::LDRXui:{
            auto op2 = MIi->getOperand(2);
            op2.print(rso);
            if (rso.str()=="target-flags(aarch64-pageoff, aarch64-nc) @__stack_chk_guard"){
              errs()<<"Operand 2: "<< rso.str() <<"\n";
              const auto &DL = MIi->getDebugLoc();
              src = MIi->getOperand(1).getReg();
              auto dst = AArch64::X24;
              if (isPrologue){
                BuildMI(MBB, ++MIi, DL, TII->get(AArch64::ADDXri), dst1).addReg(dst).addImm(MF.getFunctionNumber()).addImm(0);
                BuildMI(MBB, MIi, DL, TII->get(AArch64::PACGA), dst1).addReg(src).addReg(AArch64::SP);
                isPrologue = false;  
              }
            }
            break;
          }

          case AArch64::ADRP:{
            if (!isPrologue){
              auto op1 = MIi->getOperand(1);
              op1.print(rso);
              if (rso.str()=="target-flags(aarch64-page) @__stack_chk_guard"){
                errs()<<"Operand 1: "<< rso.str() <<"\n";
                const auto &DL = MIi->getDebugLoc();               
                BuildMI(MBB, MIi, DL, TII->get(AArch64::PACGA), dst2).addReg(src).addReg(AArch64::SP);
                BuildMI(MBB, MIi, DL, TII->get(AArch64::SUBSXrs), AArch64::XZR).addReg(dst2).addReg(dst1).addImm(0);
                //BuildMI(MBB, MIi, DL, TII->get(AArch64::BEQ)).addImm(5);
              }
            }
            break;
          }
        }
      }
    }
  }
  return true;
}
