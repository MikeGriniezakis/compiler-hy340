//
// Created by mikegriniezakis on 27/5/2024.
//

#include "vm.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "src/dispatcher/dispatcher.h"

double consts_getnumber(unsigned index) {
    // TODO: Implement this function
    return 0;
}

char* consts_getstring(unsigned index) {
    // TODO: Implement this function
    return "";
}

char* libfuncs_getused(unsigned index) {
    // TODO: Implement this function
    return "";
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
    //     int (*compfunc)(uint64_t, uint64_t);
    // struct avm_table_bucket ** arr;
    // uint hash;
    //
    //
    // hash = __hash(key);
    //
    // switch ( key->type )
    // {
    //     case undef_m:
    //
    //         /** TODO: error handling */
    //         break;
    //
    //     case number_m:
    //
    //         arr = t->numIndexed;  // warnings...
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case bool_m:
    //
    //         arr = t->boolIndexed;
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case table_m:
    //
    //         arr = t->tableIndexed;
    //         break;
    //
    //     case userfunc_m:
    //
    //         arr = &t->usrfuIndexed[0];
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case string_m:
    //     case libfunc_m:
    //
    //         arr = t->libfuIndexed;
    //         compfunc = __cmp_key_string;
    //
    //         break;
    //
    //     case nil_m:
    //
    //         /** TODO: error handling */
    //         break;
    // }
    //
    // if ( !arr[hash] )
    //     return NULL;
    //
    // struct avm_table_bucket * tmp = arr[hash];
    //
    // do {
    //
    //     if ( !(*compfunc)((uint64_t)(tmp->key.data.strVal), (uint64_t)(key->data.strVal) ) )
    //         return &tmp->value;
    //
    //     tmp = tmp->next;
    //
    // } while ( tmp );
    //
    //
    return nullptr;
}

extern avm_memcell* avm_table_set_elem(avm_table* table, avm_memcell* key, avm_memcell* value) {
    // int (*compfunc)();
    struct avm_table_bucket** arr;
    uint hash;


    // hash = __hash(key);
    //
    // switch (key->type) {
    //     case undef_m:
    //
    //         avm_error(0, "Illegal assignment to key of type undef!");
    //         break;
    //
    //     case number_m:
    //
    //         arr = t->numIndexed; // warnings...
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case bool_m:
    //
    //         arr = t->boolIndexed;
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case table_m:
    //
    //         arr = t->tableIndexed;
    //         break;
    //
    //     case userfunc_m:
    //
    //         arr = &t->usrfuIndexed[0];
    //         compfunc = __cmp_key_const;
    //
    //         break;
    //
    //     case string_m:
    //     case libfunc_m:
    //
    //         arr = t->libfuIndexed;
    //         compfunc = __cmp_key_string;
    //
    //         break;
    //
    //     case nil_m:
    //
    //         avm_error(0, "Illegal assignment to key of type nil!");
    //         break;
    // }
    //
    // if (!arr[hash]) {
    //     arr[hash] = malloc(sizeof (**arr));
    //     arr[hash]->key = *key;
    //     arr[hash]->next = NULL;
    //     arr[hash]->value = *val;
    //
    //     ++t->total;
    //
    //     return;
    // }
    //
    // struct avm_table_bucket* tmp = arr[hash];
    // struct avm_table_bucket* prev;
    //
    //
    // do {
    //     prev = tmp;
    //
    //     if (!(*compfunc)((uint64_t)(tmp->key.data.strVal), (uint64_t)(key->data.strVal))) {
    //         tmp->key = *key; /** TODO: when memclear() is called ??? */
    //         tmp->value = *val;
    //     }
    //
    //     tmp = tmp->next;
    // } while (tmp);
    //
    // prev->next = malloc(sizeof(*tmp));
    //
    // prev->next->next = NULL;
    // prev->next->key = *key;
    // prev->next->value = *val;
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
    topsp = AVM_STACKSIZE - 1;
    top = AVM_STACKSIZE - 1;
    for (int i = 0; i < globalVarCount; i++) {
        avm_stack[top].data.numVal = i;
        avm_dec_top();
        topsp--;
    }

    avm_registerlibfunc("print", libfunc_print);
    // avm_registerlibfunc("typeof", libfunc_typeof);
    // avm_registerlibfunc("totalarguments", libfunc_totalarguments);
    // avm_registerlibfunc("argument", libfunc_argument);
    // avm_registerlibfunc("objecttotalmembers", libfunc_objecttotalmembers);
    // avm_registerlibfunc("sqrt", libfunc_sqrt);
    // avm_registerlibfunc("cos", libfunc_cos);
    // avm_registerlibfunc("sin", libfunc_sin);
    // avm_registerlibfunc("strtonum", libfunc_strtonum);
    // avm_registerlibfunc("objectcopy", libfunc_objectcopy);
    // avm_registerlibfunc("objectmemberkeys", libfunc_objectmemberkeys);
    // avm_registerlibfunc("input", libfunc_input);
}

int main(int argc, char* argv[]) {
    readBinaryFile();

    while (!executionFinished) {
        execute_cycle();
    }
}
