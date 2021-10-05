#include "mips.h"
#include <stdio.h>
#include <stdlib.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)

static uint32_t reg[32];
static uint32_t pc = 0x00400000;

mips_t *mips_new(uint32_t text_size, uint32_t data_size)
{
    mips_t *m = (mips_t*)malloc(sizeof(mips_t));
    if(m == NULL) {
        debug("Cannot allocate memory\n");
        return NULL;
    }
    m->text.size = text_size;
    m->text.mem = (uint32_t*)malloc(sizeof(uint32_t)*text_size);
    if(m->text.mem == NULL) {
        debug("Cannot allocate memory\n");
        free(m);
        return NULL;
    }
    m->data.size = data_size;
    m->data.mem = (uint32_t*)malloc(sizeof(uint32_t)*data_size);
    if(m->data.mem == NULL) {
        debug("Cannot allocate memory\n");
        free(m->text.mem);
        free(m);
        return NULL;
    }
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

void mips_dump_registers()
{
    #define X(name) printf("$" #name " = 0x%08x\n", reg[name]);
    REGISTERS
    #undef X
}

void mips_step(mips_t *mips)
{
    uint8_t *addr = (uint8_t*)mips->text.mem + pc - 0x00400000;
    uint8_t opcode = *addr >> 2;
    printf("opcode = %d\t", opcode);
    for(int i = 0; i < 4; i++)
        printf("0x%02x ", *(addr++));
    printf("\n");
    pc+=4;
}