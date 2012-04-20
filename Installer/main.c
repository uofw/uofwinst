/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <psppower.h>
#include <stdio.h>
#include <string.h>
#include "kubridge.h"
#include "utils.h"

#include "galaxy.h"
#include "inferno.h"

#include "march33_660.h"

#include "popcorn.h"
#include "satelite.h"
#include "stargate.h"
#include "systemctrl.h"
#include "systemctrl_nopatch.h"
#include "usbdevice.h"
#include "vshctrl.h"
#include "recovery.h"
#include "config.h"

#include "../Common/rebootex_conf.h"

// VSH module can write F0/F1
PSP_MODULE_INFO("PROUpdater", 0x0800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

PSP_HEAP_SIZE_MAX();

#define VERSION_STR "PRO-B"
#define printf pspDebugScreenPrintf

int psp_model = 0;
u32 psp_fw_version;
int disable_smart = 0;
static u8 g_buf[64*1024] __attribute__((aligned(64)));
static u8 g_buf2[64*1024] __attribute__((aligned(64)));

void cleanup_exit(void)
{
	sceIoRemove("installer.prx");
	sceIoRemove("Rebootex.prx");
}

int compare_file_buffer(const char *path, void *file_buf, int size) 
{
	SceUID fd = -1;
	int ret;
	SceIoStat srcstat;

	ret = sceIoGetstat(path, &srcstat);
	
	if (ret != 0) {
		goto not_equal;
	}

	if (srcstat.st_size != size) {
		goto not_equal;
	}

	ret = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fd = ret;

	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		if (memcmp(g_buf, file_buf, ret)) {
			goto not_equal;
		}

		file_buf += ret;
		ret = sceIoRead(fd, g_buf, ret);
	}

	if (ret < 0) {
		goto not_equal;
	}

	sceIoClose(fd);

	return 0;

not_equal:
	if (fd >= 0)
		sceIoClose(fd);

	return 1;
}

int write_file(const char *path, unsigned char *buf, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		goto error;
	}

	ret = sceIoWrite(fd, buf, size);

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);

	return 0;
error:
	if (fd >= 0)
		sceIoClose(fd);

	return -1;
}

int smart_write_file(const char *path, unsigned char *buf, int size)
{
	int ret;

	if (!disable_smart) {
		ret = compare_file_buffer(path, buf, size);

		if (ret == 0) {
			return 0;
		}
	}

	printf("Writing %s...", path);
	ret = write_file(path, buf, size);

	if (ret == 0) {
		printf("OK\n");
	}

	return ret;
}

void init_flash()
{
	int ret;
   
	ret = sceIoUnassign("flash0:");

	while(ret < 0) {
		ret = sceIoUnassign("flash0:");
		sceKernelDelayThread(500000);
	}

	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0);
	sceIoUnassign("flash1:");
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
}

void usage(void)
{
	printf(VERSION_STR " by Team PRO\n");
}

struct InstallList {
	u8 *buf;
	uint *size;
	char *dst;
};

struct InstallList g_file_lists[] = {
	{ NULL, NULL, PATH_SYSTEMCTRL, },
	{ vshctrl, &size_vshctrl, PATH_VSHCTRL, },
	{ galaxy, &size_galaxy, PATH_GALAXY, },
	{ stargate, &size_stargate, PATH_STARGATE, },
	{ march33_660, &size_march33_660, PATH_MARCH33, },
	{ inferno, &size_inferno, PATH_INFERNO, },
	{ usbdevice, &size_usbdevice, PATH_USBDEVICE, },
	{ popcorn, &size_popcorn, PATH_POPCORN, },
	{ satelite, &size_satelite, PATH_SATELITE, },
	{ recovery, &size_recovery, PATH_RECOVERY, },
};

static const char *g_old_cfw_files[] = {
	"flash0:/kd/systemctrl.prx",
	"flash0:/kd/vshctrl.prx",
	"flash0:/kd/galaxy.prx",
	"flash0:/kd/stargate.prx",
	"flash0:/kd/march33.prx",
	"flash0:/kd/usbdevice.prx",
	"flash0:/vsh/module/satelite.prx",
};

rebootex_args g_rebootex_args = { 0, 0, { "\0" }};

void install_module(char *fname, int len)
{
    SceUID in, out;
    char inname[256] = "ms0:/uofw/";
    char outname[256] = "flash0:/_";
    strncpy(g_rebootex_args.modname[g_rebootex_args.modcount++], fname, len);

    strcat(inname, fname);
    strcat(outname, fname);
    in = sceIoOpen(inname, PSP_O_RDONLY, 0777);
    if (in < 0) {
        printf("Can't open %s: 0x%08X!\n", inname, in);
        return;
    }
    out = sceIoOpen(outname, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (out < 0) {
        printf("Can't open %s: 0x%08X!\n", outname, out);
        return;
    }
    char buf[256];
    int count;
    while ((count = sceIoRead(in, buf, 256)) > 0)
        sceIoWrite(out, buf, count);
    sceIoClose(in);
    sceIoClose(out);
}

int install_cfw(int newsysctrl)
{
	int ret;
	u32 i;

	if(disable_smart) {
		sceIoRemove("flash1:/config.se");
	}

	for(i=0; i<NELEMS(g_old_cfw_files); ++i) {
		sceIoRemove(g_old_cfw_files[i]);
	}

    if (newsysctrl) {
        g_file_lists[0].buf = systemctrl;
        g_file_lists[0].size = &size_systemctrl;
    }
    else {
        g_file_lists[0].buf = systemctrl_nopatch;
        g_file_lists[0].size = &size_systemctrl_nopatch;
    }

	for(i=0; i<NELEMS(g_file_lists); ++i) {
		ret = smart_write_file(g_file_lists[i].dst, g_file_lists[i].buf, *g_file_lists[i].size);

		if (ret != 0)
			goto exit;
	}

	// per model install goes here:
	switch(psp_model) {
		case PSP_GO:
			break;
		case PSP_4000:
			break;
		case PSP_3000:
			break;
		case PSP_2000:
			break;
		case PSP_1000:
			break;
	}

    SceUID file = sceIoOpen("ms0:/uofw/list.txt", 1, 0777);
    if (file >= 0)
    {
        char buf[256];
        char fname[256];
        int curPos = 0;
        int count;
        while ((count = sceIoRead(file, buf, 256)) > 0)
        {
	        SceCtrlData ctl;
            int i;
            for (i = 0; i < count && buf[i] != '\0' && buf[i] != '\n' && buf[i] != '\r'; i++)
                ;
            curPos += i + 1;
            strncpy(fname, buf, i);
            sceIoLseek(file, curPos, SEEK_SET);

            printf("%s: enable? (X: yes / O: no) ", fname);
            for (;;)
            {
                sceCtrlReadBufferPositive(&ctl, 1);
                if (ctl.Buttons & PSP_CTRL_CROSS)
                {
                    printf("Enabled.\n");
                    do
                    {
                        sceCtrlReadBufferPositive(&ctl, 1);
                        sceKernelDelayThread(50000);
                    } while (ctl.Buttons & PSP_CTRL_CROSS);
                    install_module(fname, i);
                    break;
                }
                else if (ctl.Buttons & PSP_CTRL_CIRCLE)
                {
                    printf("Not enabled.\n");
                    do
                    {
                        sceCtrlReadBufferPositive(&ctl, 1);
                        sceKernelDelayThread(50000);
                    } while (ctl.Buttons & PSP_CTRL_CIRCLE);
                    break;
                }
                sceKernelDelayThread(50000);
            }
        }
    }
    else
        printf("Failed opening the module list at ms0:/uofw/list.txt: 0x%08X.\nCan't load custom uOFW modules.\n", file);

	return 0;

exit:
	printf("\nWrite file error (0x%08x)! Install aborted.\n", ret);

	return -1;
}

int compare_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdd = -1;
	int ret, ret2;
	SceIoStat srcstat, dststat;

	ret = sceIoGetstat(src, &srcstat);

	if (ret != 0) {
		goto not_equal;
	}

	ret = sceIoGetstat(dst, &dststat);

	if (ret != 0) {
		goto not_equal;
	}

	if (dststat.st_size != srcstat.st_size) {
		goto not_equal;
	}

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fdd = ret;
	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		ret2 = sceIoRead(fdd, g_buf2, ret);

		if (ret2 != ret) {
			goto not_equal;
		}

		if (memcmp(g_buf, g_buf2, ret)) {
			goto not_equal;
		}

		ret = sceIoRead(fd, g_buf, ret);
	}

	if (ret < 0) {
		goto not_equal;
	}

	sceIoClose(fd);
	sceIoClose(fdd);

	return 0;

not_equal:
	if (fd >= 0)
		sceIoClose(fd);

	if (fdd >= 0)
		sceIoClose(fdd);

	return 1;
}

int copy_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdw = -1;
	int ret;

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto error;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (ret < 0) {
		goto error;
	}

	fdw = ret;
	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		ret = sceIoWrite(fdw, g_buf, ret);

		if (ret < 0) {
			goto error;
		}

		ret = sceIoRead(fd, g_buf, ret);
	}

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);
	sceIoClose(fdw);

	return 0;

error:
	sceIoClose(fd);
	sceIoClose(fdw);

	return ret;
}

int is_file_exist(const char *path)
{
	SceIoStat stat;
	int ret;

	ret = sceIoGetstat(path, &stat);

	return ret == 0 ? 1 : 0;
}

int uninstall_cfw(void)
{
	u32 ret;

	sceIoRemove("flash1:/config.se");

	u32 i;
	for(i=0; i<NELEMS(g_file_lists); ++i) {
		printf("Removing %s...", g_file_lists[i].dst);
		ret = sceIoRemove(g_file_lists[i].dst);

		if(ret == 0 || ret == 0x80010002) {
			printf("OK\n");
		} else {
			printf("failed(0x%08X)\n", ret);
		}
	}

	// per model uninstall goes here:
	switch(psp_model) {
		case PSP_GO:
			break;
		case PSP_4000:
			break;
		case PSP_3000:
			break;
		case PSP_2000:
			break;
		case PSP_1000:
			break;
	}

	return 0;
}

/**
 * mode: 0 - OFW mode, 1 - CFW mode
 */
void start_reboot(int mode)
{
	int modid, ret;
	int delay = 0;
	char modpath[80];

	sprintf(modpath, "rebootex.prx");
	modid = kuKernelLoadModule(modpath, 0, 0);

    g_rebootex_args.mode = mode;
	if (modid >= 0) {
	    SceKernelSMOption opt;
	    opt.size = 20;
	    opt.mpidstack = PSP_MEMORY_PARTITION_KERNEL;
	    opt.stacksize = 0x20000;
	    opt.priority = 0;
	    opt.attribute = 0;
		ret = sceKernelStartModule(modid, sizeof(g_rebootex_args), &g_rebootex_args, 0, &opt);

		if (ret < 0) {
			printf("start module error 0x%08x\n", ret);
			delay = 5000000;
		}
	} else {
		printf("load module error 0x%08x\n", modid);
		delay = 5000000;
	}

	cleanup_exit();
	sceKernelDelayThread(delay);
	sceKernelExitGame();
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	int ret = 0;
	struct SceIoStat stat;
	SceCtrlData ctl;
	u32 key;

	memset(&stat, 0, sizeof(stat));
	pspDebugScreenInit();
	psp_fw_version = sceKernelDevkitVersion();
	psp_model = kuKernelGetModel();
	scePowerSetClockFrequency(333, 333, 166);
	init_flash();
	usage();

	printf("Press X to launch CFW.\n");
	printf("Press Triangle to uninstall CFW.\n");
	printf("Hold L to reinstall CFW.\n");
	printf("Press R to exit.\n");

	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	while (0 == (key & (PSP_CTRL_CROSS | PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE))) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	if (key & PSP_CTRL_RTRIGGER) {
		printf("Exiting...\n");
		cleanup_exit();
		sceKernelDelayThread(100000);
		sceKernelExitGame();
	}

	switch(psp_model) {
		case PSP_GO:
			printf("PSP GO BRITE Detected ....\n");
			break;
		case PSP_9000:
			printf("PSP BRITE 3000(09g) Detected ....\n");
			break;
		case PSP_7000:
			printf("PSP BRITE 3000(07g) Detected ....\n");
			break;
		case PSP_4000:
			printf("PSP BRITE 3000(04g) Detected ....\n");
			break;
		case PSP_3000:
			printf("PSP BRITE 3000 Detected ....\n");
			break;
		case PSP_2000:
			printf("PSP SLIM 2000 Detected ....\n");
			break;
		case PSP_1000:
			printf("PSP FAT 1000 Detected ....\n");
			break;
		case PSP_11000:
			printf("PSP STREET E1000 Detected ....\n");
			break;
		default:
			printf("Unknown PSP model 0%dg\n", psp_model+1);
			break;
	}

	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons & PSP_CTRL_LTRIGGER) {
		disable_smart = 1;
	}

	if (key & PSP_CTRL_CROSS) {
		ret = install_cfw(0);

		if (ret == 0) {
			printf(" Completed.\nPress X to start CFW.\n");

			sceCtrlReadBufferPositive(&ctl, 1);
			key = ctl.Buttons;

			while (0 == (key & PSP_CTRL_CROSS)) {
				sceKernelDelayThread(50000);
				sceCtrlReadBufferPositive(&ctl, 1);
				key = ctl.Buttons;
			}

			printf("Now reboot to " VERSION_STR " :)\n");
			start_reboot(1);
		}
	} else if (key & PSP_CTRL_TRIANGLE) {
		ret = uninstall_cfw();
		printf("Now reboot to OFW :)\n");
		sceKernelDelayThread(1000000);
		start_reboot(0);
	}

	printf("Press X to exit.\n");

	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	while (0 == (key & PSP_CTRL_CROSS)) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	cleanup_exit();
	sceKernelExitGame();

	return 0;
}
