SRC	= main.c uart.c wdt.c lcd.c helper.c startup.S
NAME	= bmImage

TYPE	?= np1380
PKGCFG	= $(TYPE)/pkg.cfg
OUT	= upgrade.bin
BIN	= upgrade_$(TYPE).bin

OBJ	= $(patsubst %.S,%.o,$(SRC:%.c=%.o))

MKPKG	= ./mkpkg/mkpkg

CROSS	?= mipsel-linux-
AS	:= $(CROSS)gcc
CC	:= $(CROSS)gcc
CXX	:= $(CROSS)g++
LD	:= $(CROSS)g++
NM	:= $(CROSS)nm
OBJCOPY	:= $(CROSS)objcopy

ARGS	= -mips1 -g -O3 -mno-abicalls -fno-pic -fno-pie -nostdlib
CFLAGS	= $(ARGS) -std=gnu99
ASFLAGS	= $(ARGS)
LDFLAGS	= $(ARGS) -Xlinker --gc-sections -T kernel.ld

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
	mkimage -A mips -O linux -T kernel -C none -a 81000000 \
		-e `$(NM) -P $*.elf | grep '^_entry' | awk '{print $$3}' | grep -o '[0-9a-f]\{8\}$$'` \
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

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ -c $<

$(MKPKG):
	$(MAKE) -C mkpkg mkpkg

.PHONY: clean
clean:
	#$(MAKE) -C mkpkg clean
	rm -f $(CLEAN)

.PHONY: send
send: $(OUT)
	pv $(OUT) | ncat --send-only NP1380 1234
