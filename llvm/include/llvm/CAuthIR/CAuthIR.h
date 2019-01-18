//===----------------------------------------------------------------------===//
//
// Author: Zaheer Gauhar <zaheer.gauhar@pm.me>
// *******Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/**********
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Pass.h"

namespace llvm {

class BasicBlock;
class DominatorTree;
class Function;
class Instruction;
class Module;
class TargetLoweringBase;
class TargetMachine;
class Type;

class CAuthIR : public FunctionPass {
public:
  
private:
  const TargetMachine *TM = nullptr;

  /// TLI - Keep a pointer of a TargetLowering to consult for determining
  /// target type sizes.
  const TargetLoweringBase *TLI = nullptr;
  Triple Trip;

  Function *F;
  Module *M;

  DominatorTree *DT;

  /// \brief The minimum size of buffers that will receive stack smashing
  /// protection when -fstack-protection is used.
  unsigned SSPBufferSize = 0;


  // A prologue is generated.
  bool HasPrologue = false;

  // IR checking code is generated.
  bool HasIRCheck = false;

  /// InsertStackProtectors - Insert code into the prologue and epilogue of
  /// the function.
  ///
  ///  - The prologue code loads and stores the stack guard onto the stack.
  ///  - The epilogue checks the value stored in the prologue against the
  ///    original value. It calls __stack_chk_fail if they differ.
  bool InsertStackProtectors();

  /// CreateFailBB - Create a basic block to jump to when the stack protector
  /// check fails.
  BasicBlock *CreateFailBB();

public:
  static char ID; // Pass identification, replacement for typeid.

 /* CAuthIR() : FunctionPass(ID), SSPBufferSize(8) {
    initializeCAuthIRPass(*PassRegistry::getPassRegistry());
  }
  */

  };
}