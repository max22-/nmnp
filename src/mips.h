#ifndef MIPS_H
#define MIPS_H

#include <stdint.h>

typedef enum {NO_ERROR, SEGFAULT, NON_READABLE_SEGMENT, NON_WRITABLE_SEGMENT, NON_EXECUTABLE_SEGMENT} mips_error_t;

typedef struct {
    uint8_t *mem;
    uint32_t addr, size;
    uint8_t flags;  /* 1 : EXECUTE, 2 : WRITE, 4 : READ */
} segment_t;

typedef struct {
    segment_t *segments;
    uint32_t sgmts_num;
    uint32_t reg[32];
    uint32_t pc;
    mips_error_t error;
} mips_t;

#define REGISTERS \
    X(zero) \
    X(at) \
    X(v0) \
    X(v1) \
    X(a0) \
    X(a1) \
    X(a2) \
    X(a3) \
    X(t0) \
    X(t1) \
    X(t2) \
    X(t3) \
    X(t4) \
    X(t5) \
    X(t6) \
    X(t7) \
    X(s0) \
    X(s1) \
    X(s2) \
    X(s3) \
    X(s4) \
    X(s5) \
    X(s6) \
    X(s7) \
    X(t8) \
    X(t9) \
    X(k0) \
    X(k1) \
    X(gp) \
    X(sp) \
    X(fp) \
    X(ra)

#define X(name) name,
enum {
    REGISTERS
};
#undef X

mips_t *mips_new();
void mips_dump_registers(mips_t*);
void mips_step(mips_t*);

#endif