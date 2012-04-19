TARGET = Rebootex
OBJS = main.o ../PXE/Launcher/launcher_patch_offset.o ../Common/utils.o

CFLAGS = -O0 -G0 -Wall -Wextra -Werror -I ../Common -I ../include

PSP_FW_VERSION = 660

LDFLAGS = -mno-crt0 -nostartfiles -L ../libs
LIBS += -lpspkubridge ../CrossFW/cross_fw.a

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
