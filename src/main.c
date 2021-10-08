#include <stdio.h>
#include "mips.h"
#include "load_elf.h"


int main(int argc, char *argv[])
{
    mips_t *mips;
    printf("no mips no problem\n\n");
    if(argc < 2) {
        fprintf(stderr, "usage : %s elf_file\n", argv[0]);
        return 1;
    }
    mips = load_elf(argv[1]);

    mips_dump_registers(mips);

    for(int i =0; i < 10; i++) {
        mips_step(mips);
        if(mips->error != NO_ERROR) {
            printf("error !!!\n");
            return 1;
        }
    }

    mips_free(mips);

    return 0;
}
