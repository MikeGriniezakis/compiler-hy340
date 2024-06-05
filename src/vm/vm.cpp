//
// Created by mikegriniezakis on 27/5/2024.
//

#include "vm.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "memory/memory.h"
#include "src/dispatcher/dispatcher.h"

double consts_getnumber(unsigned index) {
    return numConsts.at(index);
}

char* consts_getstring(unsigned index) {
    return stringConsts.at(index);
}

char* libfuncs_getused(unsigned index) {
    return namedLibfuncs.at(index);
}

extern avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg) {
    if (arg->empty) {
        return nullptr;
    }

    switch (arg->type) {
        case global_a: return &avm_stack.at(AVM_STACKSIZE - 1 - arg->val);
        case local_a: return &avm_stack.at(topsp - arg->val);
        case formal_a: return &avm_stack.at(topsp + AVM_STACKENV_SIZE + 1 + arg->val);
        case retval_a: return &retval;
        case number_a: {
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;
        }
        case string_a: {
            reg->type = string_m;
            reg->data.strVal = strdup(consts_getstring(arg->val));
            return reg;
        }
        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        }
        case nil_a: {
            reg->type = nil_m;
            return reg;
        }
        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }
        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;
        }
        default: assert(0);
    }
}

void avm_dec_top() {
    if (!top) {
        fprintf(stderr, "stack overflow\n");
        executionFinished = true;
    } else {
        --top;
    }
}


extern avm_memcell* avm_table_get_elem(avm_table* table, avm_memcell* key) {
    if (key->type == number_m) {
        int hash = (int) key->data.numVal % AVM_TABLE_HASHSIZE;

        avm_table_bucket* start = table->numIndexed[hash];
        while (start != nullptr) {
            if (start->key.data.numVal == key->data.numVal) {
                return &start->value;
            }
            start = start->next;
        }

        return nullptr;
    }

    if (key->type == string_m) {
        int hash = atoi(key->data.strVal) % AVM_TABLE_HASHSIZE;

        avm_table_bucket* start = table->strIndexed[hash];
        while (start != nullptr) {
            if (start->key.data.numVal == key->data.numVal) {
                return &start->value;
            }
            start = start->next;
        }

        return nullptr;
    }

    return nullptr;
}

extern void avm_table_set_elem(avm_table* table, avm_memcell* key, avm_memcell* value) {
    if (key->type == number_m) {
        int hash = (int) key->data.numVal % AVM_TABLE_HASHSIZE;

        auto* newBucket = new avm_table_bucket();
        newBucket->key = *key;
        newBucket->value = *value;
        newBucket->next = nullptr;

        if (table->numIndexed[hash] == nullptr) {
            table->numIndexed[hash] = newBucket;
            return;
        }

        avm_table_bucket* start = table->numIndexed[hash];
        while (start->next != nullptr) {
            if (start->key.data.numVal == key->data.numVal) {
                start->value = *value;
                return;
            }
            start = start->next;
        }

        start->next = newBucket;
        table->total++;
    }

    if (key->type == string_m) {
        int hash = atoi(key->data.strVal) % AVM_TABLE_HASHSIZE;

        auto* newBucket = new avm_table_bucket();
        newBucket->key = *key;
        newBucket->value = *value;
        newBucket->next = nullptr;

        if (table->strIndexed[hash] == nullptr) {
            table->strIndexed[hash] = newBucket;
            return;
        }

        avm_table_bucket* start = table->strIndexed[hash];
        while (start->next != nullptr) {
            if (strcmp(start->key.data.strVal, key->data.strVal) == 0) {
                start->value = *value;
                return;
            }
            start = start->next;
        }

        start->next = newBucket;
        table->total++;
    }
}

void readBinaryFile() {
    std::ifstream file("/home/mikegriniezakis/CLionProjects/hy340/output.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open the binary file." << std::endl;
        return;
    }

    unsigned magic;
    file.read(reinterpret_cast<char *>(&magic), sizeof(unsigned int));
    file.read(reinterpret_cast<char *>(&globalVarCount), sizeof(unsigned int));

    unsigned int numConstsSize;
    file.read(reinterpret_cast<char *>(&numConstsSize), sizeof(unsigned int));

    for (unsigned int i = 0; i < numConstsSize; ++i) {
        double num;
        file.read(reinterpret_cast<char *>(&num), sizeof(double));
        numConsts.push_back(num);
    }

    unsigned int stringConstsSize;
    file.read(reinterpret_cast<char *>(&stringConstsSize), sizeof(unsigned int));
    for (unsigned int i = 0; i < stringConstsSize; ++i) {
        std::string str;
        char c;
        while (file.read(reinterpret_cast<char *>(&c), sizeof(char)) && c != '\0') {
            str.push_back(c);
        }

        stringConsts.push_back(strdup(str.c_str()));
    }

    unsigned int namedLibFunctionsSize;
    file.read(reinterpret_cast<char *>(&namedLibFunctionsSize), sizeof(unsigned int));
    for (unsigned int i = 0; i < namedLibFunctionsSize; ++i) {
        std::string fun;
        char c;
        while (file.read(reinterpret_cast<char *>(&c), sizeof(char)) && c != '\0') {
            fun.push_back(c);
        }
        namedLibfuncs.push_back(strdup(fun.c_str()));
    }

    unsigned int userFunctionsSize;
    file.read(reinterpret_cast<char *>(&userFunctionsSize), sizeof(unsigned int));
    for (unsigned int i = 0; i < userFunctionsSize; ++i) {
        std::string id;
        char c;
        while (file.read(reinterpret_cast<char *>(&c), sizeof(char)) && c != '\0') {
            id.push_back(c);
        }
        auto* userfunc = new struct userfunc;
        userfunc->id = strdup(id.c_str());

        file.read(reinterpret_cast<char *>(&userfunc->address), sizeof(unsigned));
        file.read(reinterpret_cast<char *>(&userfunc->localSize), sizeof(unsigned));
        userFuncs.push_back(userfunc);
    }

    unsigned int instructionsSize;
    file.read(reinterpret_cast<char *>(&instructionsSize), sizeof(unsigned int));
    for (unsigned int i = 0; i < instructionsSize; ++i) {
        auto* instruction = new struct instruction;
        file.read(reinterpret_cast<char *>(&instruction->opcode), sizeof(vmopcode));

        unsigned isResultNull;
        file.read(reinterpret_cast<char *>(&isResultNull), sizeof(unsigned));
        instruction->result.empty = (isResultNull != 0);
        if (!isResultNull) {
            file.read(reinterpret_cast<char *>(&instruction->result.type), sizeof(vmarg_t));
            file.read(reinterpret_cast<char *>(&instruction->result.val), sizeof(unsigned));
        }

        unsigned isArg1Null;
        file.read(reinterpret_cast<char *>(&isArg1Null), sizeof(unsigned));
        instruction->arg1.empty = (isArg1Null != 0);
        if (!isArg1Null) {
            file.read(reinterpret_cast<char *>(&instruction->arg1.type), sizeof(vmarg_t));
            file.read(reinterpret_cast<char *>(&instruction->arg1.val), sizeof(unsigned));
        }

        unsigned isArg2Null;
        file.read(reinterpret_cast<char *>(&isArg2Null), sizeof(unsigned));
        instruction->arg2.empty = (isArg2Null != 0);
        if (!isArg2Null) {
            file.read(reinterpret_cast<char *>(&instruction->arg2.type), sizeof(vmarg_t));
            file.read(reinterpret_cast<char *>(&instruction->arg2.val), sizeof(unsigned));
        }

        file.read(reinterpret_cast<char *>(&instruction->srcLine), sizeof(unsigned));
        instructions.push_back(instruction);
    }

    file.close();
}

void avm_initstack() {
    top = AVM_STACKSIZE - 1 - globalVarCount;
    topsp = AVM_STACKSIZE - 1;
    for (unsigned i = 0; i < AVM_STACKSIZE; i++) {
        AVM_WIPEOUT(avm_stack.at(i));
        avm_stack.at(i).type = undef_m;
    }

    // avm_registerlibfunc("input", libfunc_input);
    // avm_registerlibfunc("objectmemberkeys", libfunc_objectmemberkeys);
    // avm_registerlibfunc("objectcopy", libfunc_objectcopy);
    // avm_registerlibfunc("strtonum", libfunc_strtonum);
    // avm_registerlibfunc("sin", libfunc_sin);
    // avm_registerlibfunc("cos", libfunc_cos);
    // avm_registerlibfunc("sqrt", libfunc_sqrt);
    // avm_registerlibfunc("objecttotalmembers", libfunc_objecttotalmembers);
    // avm_registerlibfunc("argument", libfunc_argument);
    // avm_registerlibfunc("totalarguments", libfunc_totalarguments);
    avm_registerlibfunc("typeof", libfunc_typeof);
    avm_registerlibfunc("print", libfunc_print);
}

int main(int argc, char* argv[]) {
    readBinaryFile();
    avm_initstack();

    while (!executionFinished) {
        execute_cycle();
    }
}
