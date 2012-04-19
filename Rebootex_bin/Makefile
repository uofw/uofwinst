PSPSDK=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPSDK)/include -I ../Common

all: a.bin

CFLAGS=$(INCLUDES) -W -Wall -Wextra -Werror -Os -G0 -fno-pic -fno-toplevel-reorder -mno-abicalls -w -S

a.bin:
	psp-gcc $(CFLAGS) main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf rebootex.bin
	$(shell psp-config --pspdev-path)/bin/bin2c rebootex.bin rebootex.h rebootex
	psp-gcc $(CFLAGS) -DNOPATCH main.c -o main_nopatch.s
	psp-as main_nopatch.s -o main_nopatch.o
	psp-ld -T linkfile.l main_nopatch.o -o main_nopatch.elf
	psp-strip -s main_nopatch.elf
	psp-objcopy -O binary main_nopatch.elf rebootex_nopatch.bin
	$(shell psp-config --pspdev-path)/bin/bin2c rebootex_nopatch.bin rebootex_nopatch.h rebootex_nopatch

clean:
	rm -rf *~ *.s *.o *.elf rebootex.bin rebootex.h rebootex_nopatch.bin rebootex_nopatch.h
