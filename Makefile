cxx = gcc
cflags = \
		 -Os \
		 -fno-pie \
		 -fno-builtin \
	 	 -ffreestanding \
		 -mno-stack-arg-probe \
		 -fno-stack-protector \
		 -fno-asynchronous-unwind-tables \
 
ldflags = -n -s              \
          -Wl,-e,_start_s     \
		  -nostdlib -static    \
          -Wl,--gc-sections     \
          -Wl,--build-id=none    \
          -Wl,--stack,1048576     \
		  -Wl,--subsystem,windows  \

as = as
out = out
src = src

inc = -Iinclude
lib = -lkernel32

target = $(out)/main.exe
asm_out   = $(out)/assembly_output.txt
asm_debug = $(out)/assembly_debug.txt

objdump = objdump
objdumpflags_out   = -d -M intel
objdumpflags_debug = -S -M intel

src_c = $(wildcard $(src)/*.c)
src_s = $(wildcard $(src)/*.s)

obj_c = $(patsubst $(src)/%.c, $(out)/%.o, $(src_c))
obj_s = $(patsubst $(src)/%.s, $(out)/%.o, $(src_s))

objs = $(obj_c) $(obj_s)

all: $(target)

$(target): $(objs)
	$(cxx) $(objs) -o $(target) $(ldflags) $(lib)

$(out)/%.o: $(src)/%.c
	@if not exist out mkdir out
	$(cxx) -c $< -o $@ $(inc) $(cflags)

$(out)/%.o: $(src)/%.s
	@if not exist out mkdir out
	$(as) -c $< -o $@

clean:
	@if exist out\*.o del out\*.o 
	@if exist out\*.txt del out\*.txt 
	@if exist out\*.exe del /q out\*.exe

run: clean all
	./$(target)

asm_out: all
	$(objdump) $(objdumpflags_out) $(target) > $(asm_out)
asm_debug: all
	$(objdump) $(objdumpflags_debug) $(target) > $(asm_debug)