#ifndef __TERN_PATH_SLICER_TARGET_MANAGER_H
#define __TERN_PATH_SLICER_TARGET_MANAGER_H

#include "llvm/ADT/DenseSet.h"

#include "dyn-instrs.h"

namespace tern {
  class TargetMgr {
  private:
    llvm::DenseMap<void *, llvm::DenseSet<DynInstr *> *> targets;
    
  protected:

  public:
    TargetMgr();
    ~TargetMgr();
    void markTarget(void *pathId, DynInstr *dynInstr, uchar reason);
    void markTargetOfCall(DynCallInstr *dynCallInstr);
    size_t getLargestTargetIdx(void *pathId);
    void copyTargets(void *newPathId,void *curPathId);
    bool hasTarget(void *pathId);
    void clearTargets(void *pathId);
    // There could be other functionalities to be added when there are more checkers.
  };
}

#endif

