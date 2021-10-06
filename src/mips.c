#include "mips.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)

mips_t *mips_new(uint32_t text_size, uint32_t data_size)
{
    mips_t *m = (mips_t*)malloc(sizeof(mips_t));
    if(m == NULL) {
        debug("Cannot allocate memory\n");
        return NULL;
    }
    m->text.size = text_size;
    m->text.mem = (uint8_t*)malloc(text_size);
    if(m->text.mem == NULL) {
        debug("Cannot allocate memory\n");
        free(m);
        return NULL;
    }
    m->data.size = data_size;
    m->data.mem = (uint8_t*)malloc(data_size);
    if(m->data.mem == NULL) {
        debug("Cannot allocate memory\n");
        free(m->text.mem);
        free(m);
        return NULL;
    }
    memset(m->reg, 0, sizeof(m->reg));
    m->pc = 0x004000f0;
    return m;
}

uint8_t mips_load_section(section_t *section, char *bin_file)
{
    FILE *f;
    uint8_t ret = 1;
    f = fopen(bin_file, "rb");
    if(f == NULL) {
        debug("Cannot open %s\n", bin_file);
        return 0;
    }
    fread(section->mem, section->size, 1, f);
    if(!feof(f)) {
        debug("An error has occured while reading %s\n", bin_file);
        ret = 0;
    }
    fclose(f);
    return ret;
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
    uint32_t o;
    o = addr - m->text.addr;
    printf("o = %d .text.size = %d\n", o, m->text.size);
    if(o >= 0 && o < m->text.size)
        return *(uint32_t*)(m->text.mem + o);
    o = addr - m->data.addr;
    if(o >= 0 && o < m->data.size)
        return *(uint32_t*)(m->data.mem + o);
    debug("Invalid memory location\n");
    return 0;
}

static void mips_write_mem(mips_t *m, uint32_t addr, uint32_t val)
{
    uint32_t o;
    o = addr - m->data.addr;
    if(o >= 0 && o < m->data.size)
        return *(uint32_t*)(m->data.mem + o);
    debug("Invalid memory location\n");
}

void mips_step(mips_t *m)
{
    uint32_t inst = mips_read_mem(m, m->pc);
    uint8_t opcode = inst >> 26;
    printf("opcode = %d\t", opcode);
    printf("0x%08x ", inst);
    printf("\n");
    m->pc += 4;
}