//
// Created by mikegriniezakis on 27/5/2024.
//

#include "dispatcher.h"

#include <cassert>

bool executionFinished = false;
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0;
instruction* code = nullptr;
#define AVM_ENDING_PC codeSize

void execute_cycle() {
    if (executionFinished) {
        return;
    }

    if (pc == AVM_ENDING_PC) {
        executionFinished = true;
        return;
    }

    assert(pc < AVM_ENDING_PC);
    instruction* instr = code + pc;
    assert(
        instr->opcode >= 0 &&
        instr->opcode <= AVM_MAX_INSTRUCTIONS
    );

    if (instr->srcLine)
        currLine = instr->srcLine;

    unsigned oldPc = pc;
    (*executeFuncs[instr->opcode])(instr);

    if (pc == oldPc)
        ++pc;
}
