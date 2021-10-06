#include <stdio.h>
#include "mips.h"

int main(int argc, char *argv[])
{
    mips_t *mips;
    printf("no mips no problem\n\n");
    mips = mips_new(1024, 1024);
    mips_load_section(&mips->text, "./bin/hello.text.bin");
    mips->text.addr = 0x004000f0;
    mips_load_section(&mips->data, "./bin/hello.data.bin");
    mips_dump_registers(mips);

    for(int i =0; i < 10; i++)
        mips_step(mips);

    return 0;
}
