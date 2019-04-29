//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CAUTHUTILS_H
#define LLVM_CAUTHUTILS_H

#include <memory>
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "AArch64.h"
#include "AArch64RegisterInfo.h"
#include "AArch64InstrInfo.h"

#include "llvm/CAUTH/Cauth.h"

namespace llvm {

namespace CAUTH {

  class CauthUtils;

  typedef std::shared_ptr<CauthUtils> CauthUtils_ptr;

  static inline unsigned getModifierReg() { return AArch64::X24; }

  class CauthUtils {

  const TargetInstrInfo *TII;
  const TargetRegisterInfo *TRI;
  
  CauthUtils() = delete;

  public:
    CauthUtils(const TargetRegisterInfo *TRI, const TargetInstrInfo *TII);

    static inline CauthUtils_ptr get(const TargetRegisterInfo *TRI, const TargetInstrInfo *TII) {
      return std::make_shared<CauthUtils>(TRI, TII);
    };

    void convertCauthIntrinsic(MachineBasicBlock &MBB, MachineInstr &MI, unsigned instr, bool hasMod, unsigned funcID);

    void insertPAInstr(MachineBasicBlock &MBB, MachineBasicBlock::instr_iterator MIi, unsigned ptrReg,
                       unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL, bool hasMod);

    void insertPAInstr(MachineBasicBlock &MBB, MachineInstr *MI, unsigned ptrReg,
                       unsigned modReg, const MCInstrDesc &MCID, const DebugLoc &DL, bool hasMod);

    void addNops(MachineBasicBlock &MBB, MachineInstr *MI, unsigned ptrReg, unsigned modReg, const DebugLoc &DL, bool hasMod);
  };

}

}

#endif