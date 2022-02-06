CFLAGS=-Wall -std=c99 -g
CC=cc
LDFLAGS=

ASM_FILES= \
src/nt_amd64.h \
src/linux_amd64.h

OBJS= \
src/main.o \
src/parser.o \
src/cg_amd64.o

DEPS= \
src/parser.h \
src/arch.h \
src/cg.h \
src/cg_amd64.h

ahhhk: $(OBJS) src/cg.o
	$(CC) -o ahhhk $(CFLAGS) src/cg.o $(OBJS) $(LDFLAGS)

src/cg.o: %.o: %.c $(DEPS) $(ASM_FILES)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJS): %.o: %.c $(DEPS) 
	$(CC) -c $(CFLAGS) -o $@ $<

$(ASM_FILES): %.h: %.asm
	xxd -i $< $@  

clean:
	rm -f $(OBJS)
	rm -f src/cg.o
	rm -f $(ASM_FILES)
