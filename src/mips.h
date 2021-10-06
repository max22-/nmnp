#ifndef MIPS_H
#define MIPS_H

#include <stdint.h>

typedef struct {
    uint8_t *mem;
    uint32_t addr, size;
} section_t;

typedef struct {
    section_t text;
    section_t data;
    uint32_t reg[32];
    uint32_t pc;
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

mips_t *mips_new(uint32_t text_size, uint32_t data_size);
uint8_t mips_load_section(section_t *section, char *bin_file);
void mips_dump_registers(mips_t*);
void mips_step(mips_t*);

#endif