CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

ALL_HEADERS := $(shell find $(INCD) -type f -name *.h)
ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%.c, $(BLDD)/%.o, $(ALL_SRCF))

CFLAGS := -Wall -Werror -Wno-unused-function -g -O2 -DEXTRA_CREDIT
LDFLAGS := -lglib-2.0 

INC := -I$(INCD) -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

.PHONY: all clean tests

all: setup tiSH

setup:	$(BLDD)
$(BLDD):
	mkdir -p $(BLDD)

tiSH: $(ALL_OBJF) $(ALL_HEADERS)
	$(CC) -o $@ $(ALL_OBJF) $(LDFLAGS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

tests:
	$(TSTD)/test01.sh
	$(TSTD)/test02.sh
	$(TSTD)/test03.sh
# 	$(TSTD)/test04.sh

clean:
	rm -rf $(BLDD) tiSH