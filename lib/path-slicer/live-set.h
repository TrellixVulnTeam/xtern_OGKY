#ifndef __TERN_PATH_SLICER_LIVE_SET_H
#define __TERN_PATH_SLICER_LIVE_SET_H

#include <set>
#include <vector>

#include "llvm/ADT/DenseSet.h"
#include "bc2bdd/ext/JavaBDD/buddy/src/bdd.h"

#include "dyn-instrs.h"
#include "stat.h"
#include "macros.h"
#include "type-defs.h"
#include "alias-mgr.h"
#include "instr-id-mgr.h"
#include "func-summ.h"
#include "callstack-mgr.h"

namespace tern {  
  struct LiveSet {
  private:
    Stat *stat;
    AliasMgr *aliasMgr;
    InstrIdMgr *idMgr;
    FuncSumm *funcSumm;
    CallStackMgr *ctxMgr;

    /** Store all the virtual registers, they are only calling context plus the value pointer 
          of virtual registers. **/
    CtxVDenseSet virtRegs;
    /** The phiVirtRegs stores the phi nodes of the virt regs. We use this "redundant" design
    to speed up lookup. Whenever a virt reg is removed or added against the virtRegs, the same operation
    is also done agains phiVirtRegs. **/
    CtxVDenseSet phiVirtRegs;

    /** Store all the load instructions. **/
    DynInstr *loadErrnoInstr;
    llvm::DenseSet<DynInstr *> loadInstrs;
    llvm::DenseSet<DynInstr *> extCallLoadInstrs;
    bdd allLoadMem;
    bdd extCallLoadMem;

    /** Store all the pointed to locations of pointer operands of all load instructions. **/
    //RefCntBdd loadInstrsBdd;

  protected:
    void addReg(CallCtx *ctx, llvm::Value *v);
    void printVirtRegs(const char *tag);
    void printVirtReg(CtxVPair &virtReg, const char *tag);
    bool isLoadErrnoInstr(DynInstr *dynInstr);
    void cleanLoadMemBdd();

  public:
    LiveSet();
    ~LiveSet();
    void init(AliasMgr *aliasMgr, InstrIdMgr *idMgr, Stat *stat, FuncSumm *funcSumm, CallStackMgr *ctxMgr);
    size_t virtRegsSize();
    size_t loadInstrsSize();
    void clear();

    /** Virtual registers. **/
    /** Add a virtual register to live set. Skip all constants such as @x and "1".
    Each register is a hash set of <call context, static value pointer>. **/
    void addReg(DynOprd *dynOprd);
    void delReg(DynOprd *dynOprd);
    bool regIn(DynOprd *dynOprd);

    /** Get the hash set of all virtual regs in live set. **/
    CtxVDenseSet &getAllRegs();  

    /** Add all the used operands of an instruction to live set (include call args).
    It uses the addReg() above. **/
    void addUsedRegs(DynInstr *dynInstr);

    /** Handle recurive instruction such as "bitcast ...", or "load getelemptr ...", etc. **/
    void addInnerUsedRegs(CallCtx *intCtx, llvm::User *user);
 
    /** Load memory locations. **/
    /** Add a load memory location (the load instruction) to live set.
    The pointed-to locations of them are represented as refcounted-bdd. **/
    void addLoadMem(DynInstr *dynInstr);
    void addExtCallLoadMem(DynInstr *dynInstr);
                                                                          
    /** Delete a load memory location from the refcounted-bdd. **/
    void delLoadMem(DynInstr *dynInstr);     

    /** Get all load instructions. This is used in handleMem(). **/
    llvm::DenseSet<DynInstr *> &getAllLoadInstrs();    

    /** Get the abstract locations for all load memory locations.
    This is used in writtenBetween() and mayWriteFunc(). This bdd
    contains both real load instructions and sematic load from external calls. **/
    bdd getAllLoadMem(); 

    /** Get the abstract locations for all load memory locations.
    This is used in handleMem() in intra-thread phase. **/
    bdd getExtCallLoadMem();

    /** Given a calling context and a set of phi intructions, see whether this combination is in the phiVirtRegs,,
    if yes, it means phi-definition-between. **/
    bool phiDefBetween(CallCtx *ctx, InstrDenseSet *phiSet);

    /** Get the load errno instr. **/
    DynInstr *getLoadErrnoInstr();

    /** Delete the "latest" load errno instruction from live set. **/
    void delLoadErrnoInstr();
  };
}

#endif

