SRC	= main.c uart.c wdt.c
NAME	= bmImage

TYPE	?= np1380
PKGCFG	= $(TYPE)/pkg.cfg
OUT	= upgrade.bin
BIN	= upgrade_$(TYPE).bin

OBJ	= $(SRC:%.c=%.o)

MKPKG	= ./mkpkg/mkpkg

CROSS	?= mipsel-linux-
CC	:= $(CROSS)gcc
CXX	:= $(CROSS)g++
LD	:= $(CROSS)g++
NM	:= $(CROSS)nm
OBJCOPY	:= $(CROSS)objcopy

CFLAGS	= -O3 -mno-abicalls -fno-pic -fno-pie
LDFLAGS	= -static -O3 -nostdlib -mno-abicalls -fno-pic -fno-pie -T kernel.ld

.SECONDARY:
.DELETE_ON_ERROR:

CLEAN	:= $(OUT)
$(OUT): $(BIN)
	cp $< $@

CLEAN	+= $(BIN)
$(BIN): $(PKGCFG) $(MKPKG) $(NAME)
	$(MKPKG) --type=np1000 --create $< $@

CLEAN	+= $(NAME)
%: %.bin %.elf
	mkimage -A mips -O linux -T kernel -C none -a 80000000 \
		-e `$(NM) -P $*.elf | grep '^entry' | awk '{print $$3}' | grep -o '[0-9a-f]\{8\}$$'` \
		-n $(NAME)-dev -d $< $@

CLEAN	+= $(NAME).bin
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

CLEAN	+= $(NAME).elf
$(NAME).elf: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

CLEAN	+= $(OBJ)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(MKPKG):
	$(MAKE) -C mkpkg mkpkg

.PHONY: clean
clean:
	$(MAKE) -C mkpkg clean
	rm -f $(CLEAN)

.PHONY: send
send: $(OUT)
	pv $(OUT) | ncat --send-only NP1380 1234
