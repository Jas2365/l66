.text
.global _start_s
.extern _start_c

_start_s:
    subq $40, %rsp
    call _start_c
    
