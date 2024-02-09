# makefile for hello world

# to add files, simply add the c file to 'SOURCES', and the o file it'll become to 'OFILES'
SOURCES = ctest.cpp gfxpipe.cpp hw.s 3dmath.cpp 3dm.s generate.cpp fptoint.s generate_tree.cpp gen.s inlinedata.c
HEADERS = hw.h gfxpipe.h 3dmath.h generate.h floatlib.h
OFILES = ctest.o gfxpipe.o hw.o 3dmath.o 3dm.o generate.o fptoint.o gen.o inlinedata.o
BASE	= 00200000

CC = mips-scei-elf1-gcc
LD = mips-scei-elf1-ld
AS = mips-scei-elf1-gcc

CFLAGS = -mips3 -mcpu=r5900 -ffreestanding -fnonnull-objects -fno-builtin -fshort-double -mno-memcpy -nostartfiles -nostdlib -mlong64 -mhard-float -mno-abicalls -O3 -EL
LDFLAGS = -m elf32l5900 -Ttext $(BASE)
LOCAL_LIBS = libmf.a

ctest.elf: $(OFILES)
		$(LD) -o ctest.elf $(LDFLAGS) $(OFILES) $(LOCAL_LIBS)
		mips-scei-elf1-strip --strip-all ctest.elf
		@echo "done"

#ctest.o: ctest.cpp $(HEADERS)
#		 $(CC) -c -fshort-double -fno-builtin -nostartfiles -nostdlib -mlong64 -mhard-float -O0 -mips3 -mcpu=r5900 -EL $< -o $@

#generate.o: generate.cpp generate.h 3dmath.h
#		 $(CC) -c -fshort-double -fno-builtin -nostartfiles -nostdlib -mlong64 -mhard-float -O0 -mips3 -mcpu=r5900 -EL $< -o $@

%.o: %.cpp $(HEADERS)
		$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
		$(CC) -c $(CFLAGS) $< -o $@


clean:
	rm -f $(OFILES)


