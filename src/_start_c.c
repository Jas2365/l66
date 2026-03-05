#include "win.h"
#include "mem.h"
#include "defs.h"
#include "printf.h"

void _start_c() {
   if(AllocConsole()){  
        i8* k = "one";
        i32 kk = 0xa;
        i32 kk2 = 0x8;
        printf("hello world %o world\n", kk);
        printf("hello world %o world\n", kk2);
        pause();
    }

    exit(exit_success);
}