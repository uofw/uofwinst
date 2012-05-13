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
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "kubridge.h"
#include "utils.h"

#include "galaxy.h"
#include "inferno.h"

#include "march33_660.h"

#include "popcorn.h"
#include "satelite.h"
#include "stargate.h"
#include "systemctrl.h"
#include "usbdevice.h"
#include "vshctrl.h"
#include "recovery.h"
#include "config.h"

#include "../Common/rebootex_conf.h"

#define UOFW_DIR "ms0:/uofw"
#define UOFW_LIST UOFW_DIR "/list.txt"
#define UOFW_LOG UOFW_DIR "/log.txt"

// VSH module can write F0/F1
PSP_MODULE_INFO("PROUpdater", 0x0800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

PSP_HEAP_SIZE_MAX();

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
	printf("uOFW's installer, based on Team PRO's work\n");
}

struct InstallList {
	u8 *buf;
	uint *size;
	char *dst;
};

struct InstallList g_file_lists[] = {
	{ systemctrl, &size_systemctrl, PATH_SYSTEMCTRL, },
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

rebootex_args g_rebootex_args = { 0, 0, 0, { "\0" }, { "\0" }, { "\0" }};

u8 compress_buf[2000000];

typedef struct
{   
    u32     signature;  // 0
    u16     attribute; // 4  modinfo
    u16     comp_attribute; // 6
    u8      module_ver_lo;  // 8
    u8      module_ver_hi;  // 9
    char    modname[28]; // 0A
    u8      version; // 26
    u8      nsegments; // 27
    int     elf_size; // 28
    int     psp_size; // 2C
    u32     entry;  // 30
    u32     modinfo_offset; // 34
    int     bss_size; // 38
    u16     seg_align[4]; // 3C
    u32     seg_address[4]; // 44
    int     seg_size[4]; // 54
    u32     reserved[5]; // 64
    u32     devkitversion; // 78
    u32     decrypt_mode; // 7C 
    u8      key_data0[0x30]; // 80
    int     comp_size; // B0
    int     _80;    // B4
    int     reserved2[2];   // B8
    u8      key_data1[0x10]; // C0
    u32     tag; // D0
    u8      scheck[0x58]; // D4
    u32     key_data2; // 12C
    u32     oe_tag; // 130
    u8      key_data3[0x1C]; // 134
} __attribute__((packed)) PSP_Header;

typedef struct
{   
    u32 e_magic;
    u8  e_class;
    u8  e_data;
    u8  e_idver;
    u8  e_pad[9];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u32 e_entry;
    u32 e_phoff;
    u32 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} __attribute__((packed)) Elf32_Ehdr;

typedef struct
{   
    u32 p_type;                                                                                                                                                                                   
    u32 p_offset;                                                                                                                                                                                 
    u32 p_vaddr;
    u32 p_paddr;
    u32 p_filesz;
    u32 p_memsz;
    u32 p_flags;
    u32 p_align;
} __attribute__((packed)) Elf32_Phdr;

typedef struct
{
    u32 sh_name;
    u32 sh_type;
    u32 sh_flags;
    u32 sh_addr;
    u32 sh_offset;
    u32 sh_size;
    u32 sh_link;
    u32 sh_info;
    u32 sh_addralign;
    u32 sh_entsize;
} __attribute__((packed)) Elf32_Shdr;

typedef struct
{
    u16     attribute;
    u8      module_ver_lo;
    u8      module_ver_hi;
    char    modname[28];
} __attribute__((packed)) PspModuleInfo;

void generate_random(u8 *buf, int size)
{   
    int i;
    for (i = 0; i < size; i++)
        buf[i] = (rand() & 0xFF);
}

int compress_module(const char *outname, u8 *buf, int size)
{
    PSP_Header header;
    Elf32_Ehdr *elf_header;
    Elf32_Phdr *segments;
    Elf32_Shdr *sections;
    char *strtab;
    PspModuleInfo *modinfo;
    int i;

    SceUID out = sceIoOpen(outname, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (out < 0) {
        printf("Can't open %s: 0x%08X!\n", outname, out);
        return -1;
    }

    memset(&header, 0, sizeof(header));

    sceIoWrite(out, &header, sizeof(header));
    sceIoClose(out);

    // Fill simple fields
    header.signature = 0x5053507E;
    header.comp_attribute = 1;
    header.version = 1;
    header.elf_size = size;

    header._80 = 0x80;

    elf_header = (Elf32_Ehdr *)buf;
    if (elf_header->e_magic != 0x464C457F)
    {   
        if (elf_header->e_magic == 0x5053507E || elf_header->e_magic == 0x4543537E)
        {   
            printf("Already packed.\n");
            return 0;
        }
        
        printf("Not a PRX.\n");
        return -1;
    }
    
    // Fill fields from elf header
    header.entry = elf_header->e_entry;
    header.nsegments = (elf_header->e_phnum > 2) ? 2 : elf_header->e_phnum;

    if (header.nsegments == 0)
    {   
        printf("There are no segments.\n");
        return -1;
    }
    
    // Fill segements
    segments = (Elf32_Phdr *)&buf[elf_header->e_phoff];

    for (i = 0; i < header.nsegments; i++)
    {   
        header.seg_align[i] = segments[i].p_align;
        header.seg_address[i] = segments[i].p_vaddr;
        header.seg_size[i] = segments[i].p_memsz;
    }
    
    // Fill module info fields
    header.modinfo_offset = segments[0].p_paddr;
    modinfo = (PspModuleInfo *)&buf[header.modinfo_offset&0x7FFFFFFF];
    header.attribute = modinfo->attribute;
    header.module_ver_lo = modinfo->module_ver_lo;
    header.module_ver_hi = modinfo->module_ver_hi;
    strncpy(header.modname, modinfo->modname, 28);

    sections = (Elf32_Shdr *)&buf[elf_header->e_shoff];                                                                                                                                           
    strtab = (char *)(sections[elf_header->e_shstrndx].sh_offset + buf);

    for (i = 0; i < elf_header->e_shnum; i++)
    {
        if (strcmp(strtab+sections[i].sh_name, ".bss") == 0)
        {
            header.bss_size = sections[i].sh_size;
            break;
        }
    }

    if (i == elf_header->e_shnum)
    {
        if (elf_header->e_phnum >= 2)
            header.bss_size = segments[1].p_memsz - segments[1].p_filesz;
        else
            header.bss_size = 0;
    }

    if (header.attribute & 0x1000)
        header.decrypt_mode = 2;
    else if (header.attribute & 0x800)
        header.decrypt_mode = 3;
    else
        header.decrypt_mode = 4;

    header.oe_tag = 0xC01DB15D;
    header.tag = 0xDADADAF0;
    header.devkitversion = 0x06060010;

    // Fill key data with random bytes
    generate_random(header.key_data0, 0x30);
    generate_random(header.key_data1, 0x10);
    generate_random((u8 *)&header.key_data2, 4);
    generate_random(header.key_data3, 0x1C);

    gzFile comp = gzopen(outname, "ab");
    if (!comp)
    {
        printf("Cannot create temp file.\n");
        return -1;
    }

    if (gzwrite(comp, buf, size) != size)
    {
        printf("Error in compression.\n");
        return -1;
    }

    gzclose(comp);

    out = sceIoOpen(outname, PSP_O_WRONLY, 0777);
    header.psp_size = sceIoLseek(out, 0, SEEK_END);
    header.comp_size = header.psp_size - 0x150;
    sceIoLseek(out, 0, SEEK_SET);
    sceIoWrite(out, &header, sizeof(header));
    sceIoClose(out);
    return 0;
}

int install_module(char *fname)
{
    SceUID in;
    char inname[256] = UOFW_DIR "/";
    char outname[256] = "flash0:/_";

    strcat(inname, fname);
    strcat(outname, fname);
    in = sceIoOpen(inname, PSP_O_RDONLY, 0777);
    if (in < 0) {
        printf("Can't open %s: 0x%08X!\n", inname, in);
        return -1;
    }
    int count = sceIoRead(in, compress_buf, 2000000);
    sceIoClose(in);
    return compress_module(outname, compress_buf, count);
}

void replace_module(char *fname)
{
    if (install_module(fname) >= 0)
        strncpy(g_rebootex_args.replace[g_rebootex_args.replacecount++], fname, strlen(fname));
}

void add_module(char *add, char *beforeadd)
{
    if (install_module(add) >= 0)
    {
        strncpy(g_rebootex_args.add      [g_rebootex_args.addcount], add,       strlen(      add));
        strncpy(g_rebootex_args.beforeadd[g_rebootex_args.addcount], beforeadd, strlen(beforeadd));
        g_rebootex_args.addcount++;
    }
}

int is_key_press(int but)
{
	SceCtrlData ctl;
    sceCtrlReadBufferPositive(&ctl, 1);
    if (ctl.Buttons & but)
    {
        do
        {
            sceCtrlReadBufferPositive(&ctl, 1);
            sceKernelDelayThread(50000);
        } while (ctl.Buttons & but);
        return 1;
    }
    else
        return 0;
}

int query(void)
{
    for (;;)
    {
        if (is_key_press(PSP_CTRL_CROSS))
            return 1;
        else if (is_key_press(PSP_CTRL_CIRCLE))
            return 0;
        sceKernelDelayThread(50000);
    }
}

int install_cfw(void)
{
	int ret;
	u32 i;

	if(disable_smart) {
		sceIoRemove("flash1:/config.se");
	}

	for(i=0; i<NELEMS(g_old_cfw_files); ++i) {
		sceIoRemove(g_old_cfw_files[i]);
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

    SceUID file = sceIoOpen(UOFW_LIST, 1, 0777);
    typedef enum
    {
        SECTION_NONE,
        SECTION_REPLACE,
        SECTION_ADD
    } Section;
    Section sec = SECTION_NONE;
    if (file >= 0)
    {
        char buf[256];
        char arg0[256];
        char arg1[256];
        int curPos = 0;
        int count;
        while ((count = sceIoRead(file, buf, 256)) > 0)
        {
            int i;
            for (i = 0; i < count && buf[i] != '\0' && buf[i] != '\n' && buf[i] != '\r'; i++)
                ;
            curPos += i + 1;
            buf[i] = '\0';
            sceIoLseek(file, curPos, SEEK_SET);
            if (i == 0 || buf[0] == ';')
                continue;

            if (buf[0] == '[')
            {
                if (strncmp(buf, "[replace]", i) == 0)
                    sec = SECTION_REPLACE;
                else if (strncmp(buf, "[add]", i) == 0)
                    sec = SECTION_ADD;
                else
                    sec = SECTION_NONE;
            }
            else
            {
                char *spc = strchr(buf, ' ');
                if (spc == NULL)
                {
                    strncpy(arg0, buf, i);
                    arg0[i] = '\0';
                    arg1[0] = '\0';
                }
                else
                {
                    int posArg1 = spc - buf + 1;
                    int sizeArg1 = i - posArg1;
                    strncpy(arg0, buf, posArg1 - 1);
                    arg0[posArg1 - 1] = '\0';
                    strncpy(arg1, &buf[posArg1], sizeArg1);
                    arg1[sizeArg1] = '\0';
                }

                switch (sec)
                {
                case SECTION_REPLACE:
                    printf("%s: enable? (X: yes / O: no) ", arg0);
                    if (query()) {
                        printf("Enabled.\n");
                        replace_module(arg0);
                    }
                    else
                        printf("Not enabled.\n");
                    break;

                case SECTION_ADD:
                    if (arg1[0] == '\0')
                        printf("Syntax error in section [add]: no second argument!\n");
                    else
                    {
                        printf("%s: add before %s? (X: yes / O: no) ", arg0, arg1);
                        if (query()) {
                            printf("Added.\n");
                            add_module(arg0, arg1);
                        }
                        else
                            printf("Not added.\n");
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }
    else
        printf("Failed opening the module list at %s: 0x%08X.\nCan't load custom uOFW modules.\n", UOFW_LIST, file);

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
	    opt.stacksize = 0x50000;
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

	if (sceIoRemove(UOFW_LOG) < 0)
	    printf("-- Note: couldn't remove logging file.\n");

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

	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons & PSP_CTRL_LTRIGGER) {
		disable_smart = 1;
	}

	if (key & PSP_CTRL_CROSS) {
		ret = install_cfw();
		if (ret == 0) {
			printf(" Completed.\nPress X to start CFW.\n");

			sceCtrlReadBufferPositive(&ctl, 1);
			key = ctl.Buttons;

			while (0 == (key & PSP_CTRL_CROSS)) {
				sceKernelDelayThread(50000);
				sceCtrlReadBufferPositive(&ctl, 1);
				key = ctl.Buttons;
			}

			printf("Now reboot to modified firmware :)\n");
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
