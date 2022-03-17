#include "machine.h"
#include <stdlib.h>

extern const char *opcode_strs[] = {
    #define f(name) #name,
    FOR_EACH_OPCODE(f)
    #undef f
    "OP_RETURN"
};

void VM_Create(PiMachine *vm, int stack_sz, int glob_sz, char *prog) {
    vm->stack = malloc(stack_sz);
    vm->glob  = malloc(glob_sz);
    vm->prog  = prog;
}

void VM_Destroy(PiMachine *vm) {
    free(vm->stack);
    free(vm->glob);

    vm->stack = NULL;
    vm->glob  = NULL;
    vm->prog  = NULL;
}

int vm_read_prog_32(PiMachine *vm) {
    int res = *(int*)(vm->prog + vm->pc);
    vm->pc += 4;
    return res;
}

char vm_read_prog_8(PiMachine *vm) {
    return vm->prog[vm->pc++];
}

void vm_push_stack_32(PiMachine *vm, int x) {
    *((int*)vm->sp) = x;
    vm->sp += 4;
}

int vm_pop_stack_32(PiMachine *vm) {
    vm->sp -= 4;
    return *((int*)vm->sp);
}

void VM_Execute(PiMachine *vm, int at) {
    vm->pc = at;
    vm->sp = vm->stack;

    while (1) {
        char op = vm_read_prog_8(vm);

        //printf("pc = %d; sp = %d; %s\n", vm->pc - 1, (vm->sp - vm->stack), opcode_strs[op]);

        switch (op) {
        case OP_RETURN: return;
        case OP_EXTCALL: {
            int args_sz = vm_read_prog_32(vm);
            int type = *(int*)(vm->sp - 4 - args_sz);
            vm->sp -= 4 + args_sz;

            switch (type) {
            case 0: {
                int num;
                scanf("%d", &num);
                vm_push_stack_32(vm, num);
                break;
            }
            case 1: printf("%d", *(int*)(vm->sp + 4)); break;
            case 2: printf("%s", &vm->prog[*(int*)(vm->sp + 4)]); break;
            case 3: printf("%s\n", &vm->prog[*(int*)(vm->sp + 4)]); break;
            case 4: printf("%d\n", *(int*)(vm->sp + 4)); break;
            }

            break;
        }
        case OP_STORE: {
            int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm);
            *(int*)(vm->glob + a) = b;
            break;
        }
        case OP_LOAD:
            vm_push_stack_32(vm, *(int*)(vm->glob + vm_pop_stack_32(vm)));
            break;
        case OP_CONST: { vm_push_stack_32(vm, vm_read_prog_32(vm)); break; }
        case OP_CMP_NE: { vm_push_stack_32(vm, vm_pop_stack_32(vm) != vm_pop_stack_32(vm)); break; }
        case OP_CMP_EQ: { vm_push_stack_32(vm, vm_pop_stack_32(vm) == vm_pop_stack_32(vm)); break; }
        case OP_CMP_G:  { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a > b); break; }
        case OP_CMP_GE: { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a >= b); break; }
        case OP_CMP_L:  { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a < b); break; }
        case OP_CMP_LE: { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a <= b); break; }
        case OP_JMP:   { vm->pc = vm_read_prog_32(vm); break; }
        case OP_JMPZ:  { vm->pc = (vm_pop_stack_32(vm)) ? vm->pc + 4 : vm_read_prog_32(vm); break; }
        case OP_MOD:   { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a % b); break; }

        case OP_ADD:  { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a + b); break; }
        case OP_SUB:  { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a - b); break; }
        case OP_MULT: { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a * b); break; }
        case OP_DIV:  { int b = vm_pop_stack_32(vm), a = vm_pop_stack_32(vm); vm_push_stack_32(vm, a / b); break; }
        }
    }
}
