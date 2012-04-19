OBJS = main.o \
	   syspatch.o \
	   rebootex_conf.o \
	   loadercore_patch.o \
	   loadexec_patch.o \
	   startmodule_patch.o \
	   memlmd_patch.o \
	   interruptman_patch.o \
	   mediasync_patch.o \
	   sysmem_patch.o \
	   version_spoof.o \
	   usbversionfake.o \
	   march33_patch.o \
	   nid_resolver.o \
	   elf.o \
	   conf.o \
	   high_memory.o \
	   systemctrl.o \
	   kubridge.o \
	   module_handler.o \
	   usb_charge.o \
	   oe_malloc.o \
	   plugins.o \
	   psnfix.o \
	   isoload.o \
	   cpuclock.o \
	   fake_region.o \
	   msstor_cache.o \
	   systemctrl_patch_offset.o \
	   ../Common/libs.o \
	   ../Common/utils.o \
	   ../Common/strsafe.o \
	   setlongjmp.o

INCDIR = ../Common/ ../include/
CFLAGS = -O0 -G0 -Wall -Wextra -Werror -fno-pic

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += validate_stub.o ../Common/printk.o
endif

ifeq ($(NOPATCH), 1)
CFLAGS += -DNOPATCH=1
TARGET = systemctrl_nopatch
all:
	../contrib/pspgz.py systemctrl_nopatch.prx ../contrib/SystemControl.hdr systemctrl_nopatch.prx
else
TARGET = systemctrl
all:
	../contrib/pspgz.py systemctrl.prx ../contrib/SystemControl.hdr systemctrl.prx
endif

cleanobj:
	rm -f loadexec_patch.o

OBJS += nid_660_data.o
PSP_FW_VERSION = 660

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore ../CrossFW/cross_fw.a

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
