#include "mips.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

mips_t *mips_new()
{
    mips_t *m = (mips_t*)malloc(sizeof(mips_t));
    if(m == NULL) {
        debug("Cannot allocate memory\n");
        return NULL;
    }
    memset(m, 0, sizeof(mips_t));
    return m;
}

void mips_dump_registers(mips_t *m)
{
    #define X(name) printf("$" #name " = 0x%08x\n", m->reg[name]);
    REGISTERS
    #undef X
    printf("pc = 0x%08x\n", m->pc);
}

static uint32_t mips_read_mem(mips_t *m, uint32_t addr)
{
    int32_t o;
    for(int i = 0; i < m->sgmts_num; i++) {
        o = addr - m->segments[i].addr;
        if(o >= 0 && o < m->segments[i].size) {
            if(!m->segments[i].flags & 4) {
                m->error = NON_READABLE_SEGMENT;
                return 0;
            }
            return *(uint32_t*)(m->segments[i].mem + o);
        }
    }
    m->error = SEGFAULT;
    return 0;
}

/*
static void mips_write_mem(mips_t *m, uint32_t addr, uint32_t val)
{
    uint32_t o;
    o = addr - m->data.addr;
    if(o >= 0 && o < m->data.size)
        return *(uint32_t*)(m->data.mem + o);
    debug("Invalid memory location\n");
}
*/

void mips_step(mips_t *m)
{
    uint32_t inst = mips_read_mem(m, m->pc);
    uint8_t opcode = inst >> 26;
    printf("opcode = %d\t", opcode);
    printf("0x%08x ", inst);
    printf("\n");
    m->pc += 4;
}
