TARGET = pxe_launcher
OBJS = main.o imports.o launcher_patch_offset.o ../../Common/utils.o
EXTRA_CLEAN = rebootex.h installer.h

all: $(OBJS)

$(OBJS): pre-build

pre-build:
	./create_module_include.py
	touch main.c

LIBS = -lpspsystemctrl_user -lz
LIBDIR = . ../../libs
CFLAGS = -Os -G0 -Wall -I ../../Common
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
INCDIR = ../../Common/ ../../include/
ENCRYPT=1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
else
endif

OBJS += kxp_660.o
PSP_FW_VERSION = 660

BUILD_PRX = 1
PSP_EBOOT_TITLE += uOFW starter

EXTRA_TARGETS = EBOOT.PBP

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
