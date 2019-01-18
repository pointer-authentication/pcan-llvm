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
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/EHPersonalities.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/User.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

//#include "llvm/ADT/Triple.h"
//#include "llvm/IR/ValueMap.h"

//#include "llvm/CAuthIR/CAuthIR.h"

using namespace llvm;

#define DEBUG_TYPE "cauth-ir"

STATISTIC(NumFunProtected, "Number of functions protected");

static cl::opt<bool> EnableSelectionDAGSP("enable-selectiondag-cauth",
                                          cl::init(true), cl::Hidden);

//FunctionPass *llvm::createCAuthIRPass() { return new CAuthIR(); }

namespace {
  // CAuthIR - 
  struct CAuthIR : public FunctionPass {
    
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

    CAuthIR() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "CAuth-IR: ";
      errs().write_escaped(F.getName()) << '\n';
      ++NumFunProtected;
      return InsertStackProtectors();
    }
  };
}

char CAuthIR::ID = 0;
static RegisterPass<CAuthIR> X("cauth-ir", "CAuth IR Pass");

/// Create a stack guard loading and populate whether SelectionDAG SSP is
/// supported.
static Value *getStackGuard(const TargetLoweringBase *TLI, Module *M,
                            IRBuilder<> &B,
                            bool *SupportsSelectionDAGSP = nullptr) {
  if (Value *Guard = TLI->getIRStackGuard(B))
    return B.CreateLoad(Guard, true, "StackGuard");

  // Use SelectionDAG SSP handling, since there isn't an IR guard.
  //
  // This is more or less weird, since we optionally output whether we
  // should perform a SelectionDAG SP here. The reason is that it's strictly
  // defined as !TLI->getIRStackGuard(B), where getIRStackGuard is also
  // mutating. There is no way to get this bit without mutating the IR, so
  // getting this bit has to happen in this right time.
  //
  // We could have define a new function TLI::supportsSelectionDAGSP(), but that
  // will put more burden on the backends' overriding work, especially when it
  // actually conveys the same information getIRStackGuard() already gives.
  if (SupportsSelectionDAGSP)
    *SupportsSelectionDAGSP = true;
  TLI->insertSSPDeclarations(*M);
  return B.CreateCall(Intrinsic::getDeclaration(M, Intrinsic::stackguard));
}

/// Insert code into the entry block that stores the stack guard
/// variable onto the stack:
///
///   entry:
///     StackGuardSlot = alloca i8*
///     StackGuard = <stack guard>
///     call void @llvm.stackprotector(StackGuard, StackGuardSlot)
///
/// Returns true if the platform/triple supports the stackprotectorcreate pseudo
/// node.
static bool CreatePrologue(Function *F, Module *M, ReturnInst *RI,
                           const TargetLoweringBase *TLI, AllocaInst *&AI) {
  bool SupportsSelectionDAGSP = false;
  IRBuilder<> B(&F->getEntryBlock().front());
  PointerType *PtrTy = Type::getInt8PtrTy(RI->getContext());
  AI = B.CreateAlloca(PtrTy, nullptr, "StackGuardSlot");

  Value *GuardSlot = getStackGuard(TLI, M, B, &SupportsSelectionDAGSP);
  B.CreateCall(Intrinsic::getDeclaration(M, Intrinsic::stackprotector),
               {GuardSlot, AI});
  return SupportsSelectionDAGSP;
}

/// InsertStackProtectors - Insert code into the prologue and epilogue of the
/// function.
///
///  - The prologue code loads and stores the stack guard onto the stack.
///  - The epilogue checks the value stored in the prologue against the original
///    value. It calls __stack_chk_fail if they differ.
bool CAuthIR::InsertStackProtectors() {
  // If the target wants to XOR the frame pointer into the guard value, it's
  // impossible to emit the check in IR, so the target *must* support stack
  // protection in SDAG.
  bool SupportsSelectionDAGSP =      
        TLI->useStackGuardXorFP() ||    
        (EnableSelectionDAGSP && !TM->Options.EnableFastISel);
  AllocaInst *AI = nullptr;       // Place on stack that stores the stack guard.

  for (Function::iterator I = F->begin(), E = F->end(); I != E;) {
    BasicBlock *BB = &*I++;
    ReturnInst *RI = dyn_cast<ReturnInst>(BB->getTerminator());
    if (!RI)
      continue;

    // Generate prologue instrumentation if not already generated.
    if (!HasPrologue) {
      HasPrologue = true;
      SupportsSelectionDAGSP &= CreatePrologue(F, M, RI, TLI, AI);
    }

    // SelectionDAG based code generation. Nothing else needs to be done here.
    // The epilogue instrumentation is postponed to SelectionDAG.
    if (SupportsSelectionDAGSP)
      break;

    // Set HasIRCheck to true, so that SelectionDAG will not generate its own
    // version. SelectionDAG called 'shouldEmitSDCheck' to check whether
    // instrumentation has already been generated.
    HasIRCheck = true;

    // Generate epilogue instrumentation. The epilogue intrumentation can be
    // function-based or inlined depending on which mechanism the target is
    // providing.
    if (Value* GuardCheck = TLI->getSSPStackGuardCheck(*M)) {
      // Generate the function-based epilogue instrumentation.
      // The target provides a guard check function, generate a call to it.
      IRBuilder<> B(RI);
      LoadInst *Guard = B.CreateLoad(AI, true, "Guard");
      CallInst *Call = B.CreateCall(GuardCheck, {Guard});
      llvm::Function *Function = cast<llvm::Function>(GuardCheck);
      Call->setAttributes(Function->getAttributes());
      Call->setCallingConv(Function->getCallingConv());
    } else {
      // Generate the epilogue with inline instrumentation.
      // If we do not support SelectionDAG based tail calls, generate IR level
      // tail calls.
      //
      // For each block with a return instruction, convert this:
      //
      //   return:
      //     ...
      //     ret ...
      //
      // into this:
      //
      //   return:
      //     ...
      //     %1 = <stack guard>
      //     %2 = load StackGuardSlot
      //     %3 = cmp i1 %1, %2
      //     br i1 %3, label %SP_return, label %CallStackCheckFailBlk
      //
      //   SP_return:
      //     ret ...
      //
      //   CallStackCheckFailBlk:
      //     call void @__stack_chk_fail()
      //     unreachable

      // Create the FailBB. We duplicate the BB every time since the MI tail
      // merge pass will merge together all of the various BB into one including
      // fail BB generated by the stack protector pseudo instruction.
      BasicBlock *FailBB = CreateFailBB();

      // Split the basic block before the return instruction.
      BasicBlock *NewBB = BB->splitBasicBlock(RI->getIterator(), "SP_return");

      // Update the dominator tree if we need to.
      if (DT && DT->isReachableFromEntry(BB)) {
        DT->addNewBlock(NewBB, BB);
        DT->addNewBlock(FailBB, BB);
      }

      // Remove default branch instruction to the new BB.
      BB->getTerminator()->eraseFromParent();

      // Move the newly created basic block to the point right after the old
      // basic block so that it's in the "fall through" position.
      NewBB->moveAfter(BB);

      // Generate the stack protector instructions in the old basic block.
      IRBuilder<> B(BB);
      Value *Guard = getStackGuard(TLI, M, B);
      LoadInst *LI2 = B.CreateLoad(AI, true);
      Value *Cmp = B.CreateICmpEQ(Guard, LI2);
      auto SuccessProb =
          BranchProbabilityInfo::getBranchProbStackProtector(true);
      auto FailureProb =
          BranchProbabilityInfo::getBranchProbStackProtector(false);
      MDNode *Weights = MDBuilder(F->getContext())
                            .createBranchWeights(SuccessProb.getNumerator(),
                                                 FailureProb.getNumerator());
      B.CreateCondBr(Cmp, NewBB, FailBB, Weights);
    }
  }

  // Return if we didn't modify any basic blocks. i.e., there are no return
  // statements in the function.
  return HasPrologue;
}

/// CreateFailBB - Create a basic block to jump to when the stack protector
/// check fails.
BasicBlock *CAuthIR::CreateFailBB() {
  LLVMContext &Context = F->getContext();
  BasicBlock *FailBB = BasicBlock::Create(Context, "CallStackCheckFailBlk", F);
  IRBuilder<> B(FailBB);
  B.SetCurrentDebugLocation(DebugLoc::get(0, 0, F->getSubprogram()));
  if (Trip.isOSOpenBSD()) {
    Constant *StackChkFail =
        M->getOrInsertFunction("__stack_smash_handler",
                               Type::getVoidTy(Context),
                               Type::getInt8PtrTy(Context));

    B.CreateCall(StackChkFail, B.CreateGlobalStringPtr(F->getName(), "SSH"));
  } else {
    Constant *StackChkFail =
        M->getOrInsertFunction("__stack_chk_fail", Type::getVoidTy(Context));

    B.CreateCall(StackChkFail, {});
  }
  B.CreateUnreachable();
  return FailBB;
}