CHIP             = LPC810
PORT             = /dev/cu.SLAB_USBtoUART

CLASSLIB         = $(abspath ./lpc8xx_classlib)
BIN              = obj/interface.bin
SRCS            := $(abspath main.cpp)
EXTRA_FMT_SRCS  := $(abspath $(wildcard *.h))
LIBS            := $(CLASSLIB)/obj/lpc8xx_classlib.a

.PHONY: all
all: $(BIN) test

include $(CLASSLIB)/make.inc

.PHONY: test
test:
	clang++ -o test -std=gnu++17 -Wall test.cpp && ./test