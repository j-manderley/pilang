#ifndef PILANG_MACHINE_H
#define PILANG_MACHINE_H

#define FOR_EACH_OPCODE(f) \
	f(OP_EXTCALL) \
	f(OP_STORE) \
	f(OP_LOAD) \
	f(OP_CONST) \
	f(OP_CMP_NE) \
	f(OP_CMP_L) \
	f(OP_CMP_LE) \
	f(OP_CMP_EQ) \
	f(OP_CMP_GE) \
	f(OP_CMP_G) \
	f(OP_JMP) \
	f(OP_JMPZ) \
	f(OP_ADD) f(OP_SUB) \
	f(OP_MULT) f(OP_DIV) \
	f(OP_MOD) \
    f(OP_NEGATE)

enum {
    #define f(name) name,
    FOR_EACH_OPCODE(f)
    #undef f
    OP_RETURN
};

extern const char *opcode_strs[];

/*
 * EXTERNAL FUNCTION CALL
 * Stack: addr, args
 * Mem:   OP_EXTCALL, args_sz
*/

typedef struct {
	// Program counter & Stack pointer
	int pc;
	char *sp;

	char *prog, *glob;
	char *stack;
} PiMachine;

void VM_Create(PiMachine *vm, int stack_sz, int glob_sz, char *prog);
void VM_Destroy(PiMachine *vm);
void VM_Execute(PiMachine *vm, int at);

#endif
