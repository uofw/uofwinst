TARGET = satelite
OBJS = main.o blit.o ui.o menu.o clock.o umdvideo_list.o vpl.o trans.o trans_en.o stubkk.o

BUILD_PRX=1

INCDIR = ../include ../Common
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o ../Common/utils.o
endif

PSP_FW_VERSION = 660

PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user -lpspkubridge
LIBDIR = . ../libs
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
