# Changes by Nuno Neves
# Makefile by Takar

SRCS := $(wildcard *.c) $(wildcard *.s)
SRCS := $(SRCS:%.c=%)
SRCS := $(SRCS:%.s=%)

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
XILFLAGS := -mno-xl-soft-mul -mxl-barrel-shift
CFLAGS   := -g -ansi -pedantic -Ofast
LDFLAGS  := -Wl,-defsym,_STACK_SIZE=0x4000,-defsym,_HEAP_SIZE=0x4000

.PHONY: all
all: $(SRCS:%=%.elf) $(SRCS:%=%.bin) $(SRCS:%=%.dump)

.PHONY: clean
clean:
	rm -f *.bin *.dump *.elf

%.elf: %.s
	$(CC) -nostdlib $(LDFLAGS) -Wl,-Ttext=0 $< -o $@

%.elf: %.c
	$(CC) $(XILFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.dump: %.elf
	$(OBJDUMP) -DSCz $< > $@


