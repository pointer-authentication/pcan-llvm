//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/IteratedDominanceFrontier.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"

#include <queue>

#include <llvm/CAUTH/CauthIntr.h>

using namespace llvm;
using namespace CAUTH;

#define DEBUG_TYPE "test"

STATISTIC(TestCounter, "Counts number of functions greeted");

namespace {
  struct Test : public FunctionPass {
    // Pass identification, replacement for typeid
    static char ID;
    Test() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      //Get current context
      auto &C = F.getParent()->getContext();  
      DominatorTree DT = DominatorTree(F);
      
      llvm::AllocaInst *aI = nullptr;
      //errs()<<"Function Name: ";
      errs().write_escaped(F.getName()) << '\n';  //Print function name
      for (auto &BB : F){
        ++TestCounter;
        unsigned numBuffs = 0;
        errs()<<"Basic Block: ";
        errs().write_escaped(BB.getName()) << '\n';  //Print Basic block's name
        for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I){
          //I->dump();  //Dump instruction
          //Check if an alloca instruction is encountered
          if(isa<AllocaInst>(*I)) {   //Get instruction location
              ++numBuffs;
              aI = dyn_cast<llvm::AllocaInst>(&*I);
             /* if (aI->getAllocatedType()->isArrayTy()){
              Instruction *loc = I->getNextNode();  
               //Create a builder
              IRBuilder<> Builder(loc); 
              auto buffTy = Type::getInt64Ty(C);
              //Insert alloca instruction with name test_alloc
              AllocaInst* arr_alloc = Builder.CreateAlloca(buffTy , nullptr, "test_alloc");  
              ++I;
            }*/
          }
        }
        if (numBuffs>0){
         // if (BB.getName()!="entry"){
          for (auto &BBC : F) {

            auto dom=DT.dominates(&BB, &BBC);
            if (dom){
            //errs()<<"Node: "<<BB.getName()<<" dominates "<<BBC.getName()<<"\n";
            }else{
            //errs()<<"Node: "<<BB.getName()<<" doesn't dominate "<<BBC.getName()<<"\n";
            }
          }
           
            
           
           // DefiningBlocks.insert(&BB);
           //auto DF = DominanceFrontierBase<DefiningBlocks, false>();
           //DF.dump();
            DominatorTree *D;
            ForwardIDFCalculator IDFs(*D);
            SmallPtrSet<BasicBlock *, 32> DefiningBlocks;
            
            DefiningBlocks.insert(&BB);
            //DomTreeNode *dtn = DT.getNode(&BB);
            //errs()<<dtn;
            IDFs.setDefiningBlocks(DefiningBlocks);

            /*typedef std::pair<DomTreeNode *, unsigned> DomTreeNodePair;
            typedef std::priority_queue<DomTreeNodePair, SmallVector<DomTreeNodePair, 32>,
                              less_second> IDFPriorityQueue;
            IDFPriorityQueue PQ;
            for (BasicBlock *BB : DefiningBlocks) {
            if (DomTreeNode *Node = DT.getNode(BB))
              PQ.push({Node, Node->getLevel()});
              errs()<<"PQ populated";
            }*/
            
            
            
            //DefiningBlocks.push_back(SI->getParent());
            SmallVector<BasicBlock *, 32> IDFBlocks;
            errs()<<IDFBlocks.size()<<"\n";
            IDFs.calculate(IDFBlocks);
            errs()<<IDFBlocks.size()<<"\n";
            
            for (auto &IDBB : IDFBlocks){
              errs()<<"Forward IDFs for "<<BB.getName()<<"\n";
                errs().write_escaped(IDBB->getName())<<"\n";
            }

           /* PostDominatorTree *PD;
            ReverseIDFCalculator RIDFs(*PD);
            RIDFs.setDefiningBlocks(DefiningBlocks);
            SmallVector<BasicBlock *, 32> RIDFBlocks;
            RIDFs.calculate(RIDFBlocks);
            
            for (auto &IDBB : RIDFBlocks){
              errs()<<"Reverse IDFs for "<<BB.getName()<<"\n";
                errs().write_escaped(IDBB->getName())<<"\n";
            }*/

            //auto idf = IDFCalculator<ty, false>(&DT);
            //idf.calculate(BB);
            //BasicBlock *BB = /* some BB */;
            //DominanceFrontier::iterator DFI = DF->find(BB);
         
        }
        //BB.dump();
      }
      return false;
    }
  };
}

char Test::ID = 0;
static RegisterPass<Test> X("test-ir", "Test Pass");
