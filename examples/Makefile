# Changes by Nuno Neves
# Makefile by Takar

SRCS := $(wildcard *.c)
SRCS := $(SRCS:%.c=%)
SRCS := $(filter-out crt0,$(SRCS))

CC      := mb-gcc
AS      := mb-as
LD      := mb-ld
OBJCOPY := mb-objcopy
OBJDUMP := mb-objdump

#Xilinx Compiler flags (XILFLAGS): 
#-mxl-soft-div -> Use Soft Divider
#-msoft-float -> Use Soft Floating-Point
#-mxl-barrel-shift -> Use Hardware Barrel Shifter
#-mno-xl-soft-mul -> Use Hardware Multiplier
XILFLAGS := -mxl-soft-mul -mxl-soft-div -mxl-barrel-shift
CFLAGS   := -ansi -pedantic -O0 
LDFLAGS  := -Wl,-defsym,_STACK_SIZE=0x4000 -Wl,-defsym,_HEAP_SIZE=0x4000

.PHONY: all
all: $(SRCS:%=%.elf) $(SRCS:%=%.S) $(SRCS:%=%.bin) $(SRCS:%=%.dump)

.PHONY: clean
clean:
	rm -f *.bin *.dump *.elf *.S

%.S: %.c
	$(CC) $(XILFLAGS) $(CFLAGS) $(LDFLAGS) -S $< -o $@

%.elf: %.c
	$(CC) $(XILFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.dump: %.elf
	$(OBJDUMP) -DSCz $< > $@


