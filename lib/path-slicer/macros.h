#ifndef __TERN_PATH_SLICER_DBG_PRINT_H
#define __TERN_PATH_SLICER_DBG_PRINT_H

#define DBG0 0 /* No dbg print. */
#define DBG1 1 /* Dbg print everything. */
#define DBG2 2 /* Dbg print only some important events. */

#include "options.h"
#include "path_slicer-options.h"
#define SERRS if(get_option(tern_path_slicer,print_debug_info)==DBG1)errs()
#define SERRS2 if(get_option(tern_path_slicer,print_debug_info)==DBG2)errs()

#define GETTIME(clock) gettimeofday(&clock,NULL)
#define ADDTIME(num, clock_st, clock_end) \
  num+=(clock_end.tv_sec-clock_st.tv_sec)+(clock_end.tv_usec-clock_st.tv_usec)/1000000.0
#define BEGINTIME(clock) GETTIME(clock)
#define ENDTIME(num, clock_st, clock_end) GETTIME(clock_end);ADDTIME(num, clock_st, clock_end)

#define NORMAL_SLICING (get_option(tern_path_slicer,slicing_mode)==0)
#define MAX_SLICING (get_option(tern_path_slicer,slicing_mode)==1)
#define RANGE_SLICING (get_option(tern_path_slicer, slicing_mode)==2)

#define KLEE_RECORDING (get_option(tern_path_slicer, trace_util_type)==0)
#define XTERN_RECORDING (get_option(tern_path_slicer, trace_util_type)==1)

#define SIZE_T_INVALID (size_t(-1))
#define ASSERT(stmt) if(get_option(tern_path_slicer,print_debug_info)!=DBG0)assert(stmt)

#define CTX_SENSITIVE (get_option(tern_path_slicer, context_sensitive_ailas_query)==1)

#include <ext/hash_set>
#include <ext/hash_map>
//#define HSET __gnu_cxx::hash_set
#define HMAP __gnu_cxx::hash_map
#define HM_IN(ELEM, SET) (SET.find(ELEM) != SET.end())
//#define HS_IN(ELEM, SET) HM_IN(ELEM, SET)
#define DM_IN(ELEM, SET) (SET.count(ELEM) > 0)
#define DS_IN(ELEM, SET) DM_IN(ELEM, SET)
#define NOT_TAKEN_INSTR "NOT TAKEN"

#endif

