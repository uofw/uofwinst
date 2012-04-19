TARGET_LIB = cross_fw.a
OBJS = cross_fw.o

IMPORTS = ../Imports/SysMemForKernel.o \
		  ../Imports/LoadCoreForKernel.o \
		  ../Imports/SysMemForKernel.o \
		  ../Imports/ModuleMgrForKernel.o \
		  ../Imports/InterruptManagerForKernel.o \
		  ../Imports/sceResmgr_driver.o \
		  ../Imports/InitForKernel.o \
		  ../Imports/LoadExecForKernel.o \
		  ../Imports/sceCtrl_driver.o

PSP_FW_VERSION = 660

OBJS += $(IMPORTS)

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
