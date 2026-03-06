#include "Wwin.h"
#include "Wmem.h"
#include "Wdefs.h"
#include "Wprintf.h"
#include "Wmath.h"

void _start_c() {
   if(AllocConsole()){
    
        const i8* var = "apple";
        f32 val = 0.236;
        i32 vali = -1;
        printf("Val:%.2s %+.3f  %o val \nval\n", var, val, vali);
        
        pause();
    }

    exit(exit_success);
}