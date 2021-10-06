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
            if(!(m->segments[i].flags & 4)) {
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

static uint32_t sign_extend(uint32_t val, uint8_t bit_count)
{
    uint8_t sign_bit = (val >> (bit_count -1)) & 1;
    if(sign_bit)
        val |= 0xFFFFFFFF << bit_count;
    return val;
}

static void syscall(mips_t *m)
{
    switch(m->reg[v0]) {
        case 1:
            printf("%d", m->reg[a0]);
            break;
        default:
            debug("Unknown syscall\n");
            break;
    }
}

void mips_step(mips_t *m)
{
    uint32_t inst = mips_read_mem(m, m->pc);
    uint8_t opcode = inst >> 26;
    uint8_t function = inst & 0x3f;
    uint32_t j_addr;
    uint8_t rs, rt;
    uint32_t imm;
    printf("0x%08x ", m->pc);
    switch(opcode) {
        case OP_R_TYPE:
            switch(function) {
                case FUNC_SYSCALL:
                    printf("syscall\n");
                    syscall(m);
                    m->pc += 4;
                    break;
                default:
                    printf("R-type instruction\n");
                    break;
            }
            break;
        case OP_ADDIU:
            rs = inst >> 21 & 0x1f;
            rt = inst >> 16 & 0x1f;
            imm = sign_extend(inst & 0xFFFF, 16);
            m->reg[rt] = m->reg[rs] + imm;
            printf("addiu %d, %d, %d\n", rt, rs, imm);
            m->pc += 4;
            break;
        case OP_J:
            printf("j ");
            j_addr = 4*(inst & ~(0x3F << 26));
            m->pc = j_addr;
            printf("0x%08x\n", 4*j_addr);
            #warning Check if address calculation is correct
            break;
        case OP_JAL:
            printf("jal\n");
            #warning not implemented yet
            break;
        default:
            printf("I-type instruction\n");
            break;


    }
    /*printf("opcode = %d\t", opcode);
    printf("0x%08x ", inst);
    printf("\n");*/
    //m->pc += 4;
}
