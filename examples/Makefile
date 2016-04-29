# Changes by Nuno Neves
# Makefile by Takar

SRCS := $(wildcard *.c)
SRCS := $(SRCS:%.c=%)
SRCS := $(filter-out crt0,$(SRCS))

CC      := gcc
AS      := as
LD      := ld
OBJCOPY := objcopy
OBJDUMP := objdump

#Xilinx Compiler flags (XILFLAGS):
#-mxl-soft-div -> Use Soft Divider
#-msoft-float -> Use Soft Floating-Point
#-mxl-barrel-shift -> Use Hardware Barrel Shifter
#-mno-xl-soft-mul -> Use Hardware Multiplier
XILFLAGS := -mno-xl-soft-mul -mxl-soft-div -mxl-barrel-shift
CFLAGS   := -std=c99 -pedantic -Ofast
LDFLAGS  := -Wl,-defsym,_STACK_SIZE=0x4000 -Wl,-defsym,_HEAP_SIZE=0x4000

.PHONY: all
all: intel mb

.PHONY: intel
intel: $(SRCS:%=%.intel) $(SRCS:%=%.intel.dump) $(SRCS:%=%.intel.bin) 

.PHONY: mb
mb: $(SRCS:%=%.mb) $(SRCS:%=%.mb.dump) $(SRCS:%=%.mb.bin) 

.PHONY: clean
clean:
	rm -f *.bin *.dump *.intel *.mb

%.intel: %.c
	$(CC) $(CFLAGS) -Dxil_printf=printf $< -o $@

%.intel.bin: %.intel
	$(OBJCOPY) -O binary $< $@

%.intel.dump: %.intel
	$(OBJDUMP) -DSCz $< > $@

%.mb: %.c
	mb-$(CC) $(XILFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

%.mb.bin: %.mb
	mb-$(OBJCOPY) -O binary $< $@

%.mb.dump: %.mb
	mb-$(OBJDUMP) -DSCz $< > $@
