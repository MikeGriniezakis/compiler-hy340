//
// Created by mikegriniezakis on 27/5/2024.
//

#ifndef DISPATCHER_H
#define DISPATCHER_H
#include "src/targetCode/targetCode.h"
#include "src/vm/vm.h"

#define AVM_MAX_INSTRUCTIONS (unsigned) 25
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

typedef void (*execute_func_t)(instruction*);

typedef void (*library_func_t)();
library_func_t avm_get_library_func(char* id);

inline bool executionFinished = false;
inline unsigned pc = 0;
inline unsigned currLine = 0;
#define AVM_ENDING_PC instructions

extern void execute_assign (instruction*);
extern void avm_assign(avm_memcell* lv, avm_memcell* rv);

inline double add_impl(double x, double y) { return x+y;};
inline double sub_impl(double x, double y) { return x-y;};
inline double mul_impl(double x, double y) { return x*y;};
inline double div_impl(double x, double y) { return x/y;};
inline double mod_impl(double x, double y) { return (unsigned) x % (unsigned) y;};

typedef double (*arithmetic_func_t)(double x, double y);
inline arithmetic_func_t arithmeticFuncs[] = {
        add_impl,
        sub_impl,
        mul_impl,
        div_impl,
        mod_impl
};

extern void execute_add (instruction*);
extern void execute_sub (instruction*);
extern void execute_mul (instruction*);
extern void execute_div (instruction*);
extern void execute_mod (instruction*);

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

extern void execute_arithmetic(instruction* instr);

extern void execute_uminus (instruction*);
extern void execute_and (instruction*);
extern void execute_or (instruction*);
extern void execute_not (instruction*);

inline bool jle_impl(double x, double y) { return x <= y;};
inline bool jge_impl(double x, double y) { return x >= y;};
inline bool jlt_impl(double x, double y) { return x < y;};
inline bool jgt_impl(double x, double y) { return x > y;};
inline bool jeq_impl(double x, double y) { return x == y;};
inline bool jne_impl(double x, double y) { return x != y;};

typedef bool (*cmp_func) (double, double);
inline cmp_func comparisonFuncs[] = {
        jeq_impl,
        jne_impl,
        jle_impl,
        jge_impl,
        jlt_impl,
        jgt_impl
};

extern void execute_jeq (instruction*);
extern void execute_jne (instruction*);
extern void execute_jle (instruction*);
extern void execute_jge (instruction*);
extern void execute_jlt (instruction*);
extern void execute_jgt (instruction*);

#define execute_jle execute_comparison
#define execute_jge execute_comparison
#define execute_jlt execute_comparison
#define execute_jgt execute_comparison

extern void execute_comparison(instruction* instr);

inline unsigned totalActuals = 0;
extern void execute_call (instruction*);
extern void execute_pusharg (instruction*);
extern void execute_funcenter (instruction*);
extern void execute_funcexit (instruction*);
extern void avm_call_lib_func(char* funcName);
extern void avm_call_save_env();
extern void avm_push_env_value(unsigned val);
extern unsigned avm_get_env_value(unsigned i);
extern userfunc* avm_get_func_info(unsigned pc);
extern void avm_totalactuals();
extern avm_memcell* avm_getactual(unsigned i);
extern void libfunc_print();
extern void avm_registerlibfunc(char* id, library_func_t addr);

extern void execute_newtable (instruction*);
extern void execute_tablegetelem (instruction*);
extern void execute_tablesetelem (instruction*);
extern void execute_nop (instruction*);

inline execute_func_t executeFuncs[] = {
        execute_assign,
        execute_mul,
        execute_uminus,
        execute_not,
        execute_jle,
        execute_jgt,
        execute_funcenter,
        execute_tablegetelem,
        execute_add,
        execute_div,
        execute_and,
        execute_jeq,
        execute_jge,
        execute_call,
        execute_funcexit,
        execute_tablesetelem,
        execute_sub,
        execute_mod,
        execute_or,
        execute_jne,
        execute_jlt,
        execute_pusharg,
        execute_newtable,
        execute_nop
};

extern void execute_cycle();

#endif //DISPATCHER_H
