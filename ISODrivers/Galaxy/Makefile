all:
	../../contrib/pspgz.py galaxy.prx ../../contrib/SystemControl.hdr galaxy.prx PROGalaxyController 0x1006

TARGET = galaxy
OBJS = main.o galaxy_patch_offset.o isocache.o imports.o
OBJS += SystemCtrlForKernel.o

CFLAGS = -Os -G0 -Wall -I. -I../../include -I../../Common

LDFLAGS = -mno-crt0 -nostartfiles -L. -L../../libs

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o \
		../../Common/utils.o
endif

PSP_FW_VERSION = 660

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PRX_EXPORTS = export.exp
LIBS += -lpspsystemctrl_kernel -lpspsemaphore 

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
