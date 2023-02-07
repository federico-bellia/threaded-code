#include <iostream>
#include <setjmp.h>

enum INSTR {
    EXIT = 0,
    ADD,
    SUB,
    OUT,
    LOAD,
    NEST,
    UNNEST,
    IFEQ,
    IFGT,
    JUMP,
    INSTR_COUNT
};

struct Interpreter {
    int ip;
    int w;
    int sp;
    int callStack[256];
    int registers[4];
    int* data;
    int* program;
    jmp_buf addresses[INSTR_COUNT];
    
    void next() {
        w = program[ip++];
        longjmp(addresses[(INSTR)program[w]], 1);
    }
    
    void pushIp() {
        callStack[sp++] = ip;
    }
    
    void popIp() {
        ip = callStack[sp--];
    }
    
    int arg(int i) {
        int v = program[w + 1 + i];
        
        return v >= 4 ? data[v - 4] : registers[v];
    }
    
    int* rwarg(int i) {
        int v = program[w + 1 + i];
        
        return v >= 4 ? nullptr : registers + v;
    }
};

static int data[] { 50, 25 };
static int program[] {
    EXIT, 11, 15, 0, 0, 0, 0, 0, 0, 0, 0, SUB, 4, 5, 0, OUT, 5
};

static int gcdData[] { 75, 123, 39, 37, 31, };
static int gcdProgram[] {
    EXIT, LOAD, 4, 0, LOAD, 5, 1, IFEQ, 0, 1, JUMP, 6, IFGT, 0, 1, JUMP, 7, SUB, 1, 0, 1, JUMP, 8, SUB, 0, 1, 0, OUT, 1,
    1, 4, 7, 10, 12, 15, 17, 21, 23, 21, 27, 0, 0, 0, 0, 0, 0 };

int main() {
    Interpreter i{};
    i.data = gcdData;
    i.program = gcdProgram;
    i.ip = 29;

    if(setjmp(i.addresses[EXIT])) {
        return 0;
    }
    
    if(setjmp(i.addresses[ADD])) {
        int l = i.arg(0);
        int r = i.arg(1);
        int* res = i.rwarg(2);
        
        *res = l + r;
        
        i.next();
    }
    
    if(setjmp(i.addresses[SUB])) {
        int l = i.arg(0);
        int r = i.arg(1);
        int* res = i.rwarg(2);
        
        *res = l - r;

        i.next();
    }
    
    if(setjmp(i.addresses[OUT])) {
        std::cout << i.arg(0);
        i.next();
    }
    
    if(setjmp(i.addresses[LOAD])) {
        int v = i.arg(0);
        int* dest = i.rwarg(1);
        
        *dest = v;
        i.next();
    }
    
    if(setjmp(i.addresses[NEST])) {
        i.pushIp();
        i.ip = i.w + 1;
        i.next();
    }
    
    if(setjmp(i.addresses[UNNEST])) {
        i.popIp();
        i.next();
    }
    
    if(setjmp(i.addresses[IFEQ])) {
        int l = i.arg(0);
        int r = i.arg(1);
        
        i.ip += (l == r ? 0 : 1);
        i.next();
    }
    
    if(setjmp(i.addresses[IFGT])) {
        int l = i.arg(0);
        int r = i.arg(1);
        
        i.ip += (l > r ? 0 : 1);
        i.next();
    }
    
    if(setjmp(i.addresses[JUMP])) {
        int addr = i.arg(0);
        
        i.ip = addr;
        i.next();
    }
    
    i.next();
    return 0;
}
