# Changes by Nuno Neves
# Makefile by Takar

SRCS := $(wildcard *.c)
CC   := mb-gcc

#Xilinx Compiler flags (XILFLAGS): 
#-mxl-soft-div -> Use Soft Divider
#-msoft-float -> Use Soft Floating-Point
#-mxl-barrel-shift -> Use Hardware Barrel Shifter
#-mno-xl-soft-mul -> Use Hardware Multiplier
XILFLAGS := -mno-xl-soft-mul -mxl-barrel-shift
CFLAGS   := -g -ansi -pedantic -Ofast
LDFLAGS  := -Wl,-defsym,_STACK_SIZE=0x4000,-defsym,_HEAP_SIZE=0x4000

.PHONY: all
all: $(SRCS:%.c=%.elf) $(SRCS:%.c=%.bin) $(SRCS:%.c=%.dump)

.PHONY: clean
clean:
	rm -f *.bin *.dump *.elf

%.elf: %.c
	$(CC) $(XILFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

%.bin: %.elf
	mb-objcopy -O binary $< $@

%.dump: %.elf
	mb-objdump -DSCz $< > $@


