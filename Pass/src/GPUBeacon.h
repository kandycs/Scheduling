#ifndef _GPU_BEACON_H_
#define _GPU_BEACON_H_

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils.h>

#include "Visitor.h"

using namespace llvm;

class GPUBeaconPass : public ModulePass {
 private:
  CUDAVisitor CUDAInfo;
  std::vector<CUDATask> Tasks;
  std::set<CallInst *> getMemAllocOps(InvokeInfo II);
  std::set<CallInst *> getMemFreeOps(InvokeInfo II);
  CallInst *getGridCtor(InvokeInfo II);
  CallInst *getBlockCtor(InvokeInfo II);

  bool needTobeMerged(CUDATask &A, CUDATask &B);
  bool dominate(MemAllocInfo &MAI, InvokeInfo &II);
  bool dominate(GridCtorInfo &GCI, InvokeInfo &II);
  bool dominate(InvokeInfo &II, MemFreeInfo &MFI);
  bool dominate(CallInst *C1, CallInst *C2);

  void buildCUDATasks(Module &M);

  int getDistance(Instruction *S, Instruction *E);

 public:
  static char ID;

  GPUBeaconPass() : ModulePass(ID) {}
  virtual bool runOnModule(Module &M);

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    // AU.addRequired<LoopInfoWrapperPass>();
    // AU.addRequired<ScalarEvolutionWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequiredID(LoopSimplifyID);
    AU.addRequiredID(LCSSAID);
  }
};

#endif