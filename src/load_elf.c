#include <stdio.h>
#include <stdlib.h>
#include "load_elf.h"
#include "debug.h"


typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

static uint8_t check_header(Elf32_Ehdr *ehdr)
{
    if(ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
        debug("Not an ELF file\n");
        return 0;
    }
    if(ehdr->e_ident[4] != 1) {
        debug("Only 32 bit class is supported\n");
        return 0;
    }
    if(ehdr->e_ident[5] != 1) {
        debug("Only LSB encoding is supported\n");
        return 0;
    }
    if(ehdr->e_ident[6] != 1) {
        debug("Invalid ELF version\n");
        return 0;
    }
    
    if(ehdr->e_type != 2) {
        debug("Only executable files are supported");
        return 0;
    }

    if(ehdr->e_machine != 8) {
        debug("Architecture not supported");
        return 0;
    }
    return 1;
}

mips_t *load_elf(char *file)
{
    mips_t *m = NULL;
    FILE *f;
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr;
    f = fopen(file, "rb");
    if(f == NULL) {
        debug("Cannot open %s\n", file);
        return 0;
    }
    fread(&ehdr, sizeof(Elf32_Ehdr), 1, f);
    if(!check_header(&ehdr)) {
        fclose(f);
        return NULL;
    }

    printf("phdr table entries = %d\n", ehdr.e_phnum);
    printf("entry_point = 0x%08x\n", ehdr.e_entry);

    m = mips_new();

    for(int i = 0; i < ehdr.e_phnum; i++) {
        fseek(f, sizeof(Elf32_Ehdr) + i * sizeof(Elf32_Phdr), SEEK_SET);
        fread(&phdr, sizeof(Elf32_Phdr), 1, f);
        if(phdr.p_type == 1) { /* LOAD */
            printf("Loading segment #%d\n", i);
            uint32_t curr_sgmt = m->sgmts_num;
            m->sgmts_num++;
            m->segments = (segment_t*)realloc(m->segments, sizeof(segment_t) * m->sgmts_num);
            if(m->segments == NULL) {
                debug("Cannot allocate memory");
                fclose(f);
                return NULL;
            }
            printf("vaddr = 0x%08x\t", phdr.p_vaddr);
            m->segments[curr_sgmt].addr = phdr.p_vaddr;
            printf("size = %d\t", phdr.p_memsz);
            m->segments[curr_sgmt].size = phdr.p_memsz;
            printf("flags = ");
            printf("%c", phdr.p_flags & 4 ? 'R' : '_');
            printf("%c", phdr.p_flags & 2 ? 'W' : '_');
            printf("%c", phdr.p_flags & 1 ? 'X' : '_');
            printf("\n");
            m->segments[curr_sgmt].flags = phdr.p_flags;
            m->segments[curr_sgmt].mem = (uint8_t*)malloc(phdr.p_memsz);
            if(m->segments[curr_sgmt].mem == NULL) {
                debug("Cannot allocate memory");
                fclose(f);
                return NULL;
            }
            fseek(f, phdr.p_offset, SEEK_SET);
            fread(m->segments[curr_sgmt].mem, phdr.p_filesz, 1, f);
        }
    }
    m->pc = ehdr.e_entry;
    m->error = NO_ERROR;
    fclose(f);
    return m;
}