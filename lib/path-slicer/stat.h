#ifndef __TERN_PATH_SLICER_STAT_H
#define __TERN_PATH_SLICER_STAT_H

#include <string>

#include "llvm/Instruction.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Module.h"

#include "macros.h"

namespace tern {
  class DynInstr;
  class DynCallInstr;
  class InstrIdMgr;
  class CallStackMgr;
  class FuncSumm;
  class Stat {
  private:
    InstrIdMgr *idMgr;
    CallStackMgr *ctxMgr;
    FuncSumm *funcSumm;
    llvm::DenseMap<const llvm::Instruction *, llvm::raw_string_ostream * > buf;

    /* The set of static instructions and executed instructions in a module. */
    llvm::DenseSet<const llvm::Instruction *> staticInstrs;
    llvm::DenseSet<const llvm::Instruction *> exedStaticInstrs;

    /* The set of static external function calls. */
    llvm::DenseSet<const llvm::Instruction *> externalCalls;

    /* Path exploration branches frequency, map from instruction to frequency. */
    llvm::DenseMap<llvm::Instruction *, int> pathFreq;
  protected:
    void collectExternalCalls(DynCallInstr *dynCallInstr);
    void collectExedStaticInstrs(DynInstr *dynInstr);
    void printFileLoc(llvm::raw_ostream &S, const llvm::Instruction *instr);

  public:
    struct timeval interSlicingSt;
    struct timeval interSlicingEnd;
    double interSlicingTime;

    struct timeval intraSlicingSt;
    struct timeval intraSlicingEnd;
    double intraSlicingTime;

    struct timeval intraChkTgtSt;
    struct timeval intraChkTgtEnd;
    double intraChkTgtTime;

    struct timeval intraPhiSt;
    struct timeval intraPhiEnd;
    double intraPhiTime;

    struct timeval intraBrSt;
    struct timeval intraBrEnd;
    double intraBrTime;

    struct timeval intraRetSt;
    struct timeval intraRetEnd;
    double intraRetTime;

    struct timeval intraCallSt;
    struct timeval intraCallEnd;
    double intraCallTime;

    struct timeval intraMemSt;
    struct timeval intraMemEnd;
    double intraMemTime;

    struct timeval intraNonMemSt;
    struct timeval intraNonMemEnd;
    double intraNonMemTime;



    
    Stat();
    ~Stat();
    void init(InstrIdMgr *idMgr, CallStackMgr *ctxMgr, FuncSumm *funcSumm);
    void printStat(const char *tag);
    const char *printInstr(const llvm::Instruction *instr, bool withFileLoc = false);    
    void printDynInstr(DynInstr *dynInstr, const char *tag,
      bool withFileLoc = false);
    void printDynInstr(llvm::raw_ostream &S, DynInstr *dynInstr, const char *tag,
      bool withFileLoc = false);

    /* Collect statistics on the set of all static instructions and 
    the set of all executed instructions. */
    void collectStaticInstrs(llvm::Module &M);
    size_t sizeOfStaticInstrs();
    size_t sizeOfExedStaticInstrs();

    /* For collecting external calls (for reference of function summary). */
    void printExternalCalls();

    /* Collect stat of executed instrutions. */
    void collectExed(DynInstr *dynInstr);

    /* Utility for explored path frequency. */
    void collectExplored(llvm::Instruction *instr);
    void printExplored();
  };
}

#endif

