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
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <string.h>
#include <zlib.h>
#include "systemctrl.h"
#include "rebootex.h"
#include "utils.h"
#include "printk.h"

#include "installer.h"
#include "Rebootex_prx.h"
#include "launcher_patch_offset.h"
#include "rebootex_conf.h"

#define INTR(intr) \
	_sw((intr), address); address +=4;
#define INTR_HIGH(intr) \
	_sw((intr&0xFFFF0000) + ((((intr) + (data_address >> 16)) & 0xFFFF)), address); address +=4;
#define INTR_LOW(intr) \
	_sw((intr&0xFFFF0000) + (((intr) + (data_address & 0xFFFF)) & 0xFFFF), address); address +=4;

PSP_MODULE_INFO("PXELauncher", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(128);

//installer path
char installerpath[256];

//psp model
int psp_model = 0;

u32 psp_fw_version = 0;

//load reboot function
int (* LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

extern int sceKernelPowerLock(unsigned int, unsigned int);

u8 decompress_buf[1024*1024L];

void do_exploit_639(void);
void do_exploit_660(void);

int scePowerRegisterCallbackPrivate_635(unsigned int slot, int cbid);
int scePowerUnregisterCallbackPrivate_635(unsigned int slot);
int scePowerRegisterCallbackPrivate_620(unsigned int slot, int cbid);
int scePowerUnregisterCallbackPrivate_620(unsigned int slot);
int SysMemUserForUser_D8DE5C1E(int arg1, int arg2, int (* callback)(void), int arg4, int branchkiller);

void build_rebootex_configure(void)
{
	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG);

	conf->magic = REBOOTEX_CONFIG_MAGIC;
	conf->psp_model = psp_model;
	conf->rebootex_size = size_rebootex;
	conf->psp_fw_version = psp_fw_version;
}

//load reboot wrapper
int _LoadReboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((char *)REBOOTEX_START, rebootex, size_rebootex);

	//reset reboot flags
	memset((char *)REBOOTEX_CONFIG, 0, 0x100);
	//copy installer path
	memcpy((char *)REBOOTEX_CONFIG_ISO_PATH, installerpath, sizeof(installerpath));

	build_rebootex_configure();

	//forward
	return LoadReboot(arg1, arg2, arg3, arg4);
}

//our 6.35 kernel permission call
int kernel_permission_call(void)
{
	struct sceLoadExecPatch *patch;

	//cache invalidation functions
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelIcacheInvalidateAll);
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelDcacheWritebackInvalidateAll);

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//LoadCoreForKernel_EF8A0BEA
	SceModule2 * (* _sceKernelFindModuleByName)(const char * libname) = (void *)g_offs->sceKernelFindModuleByName;

	//find LoadExec module
	SceModule2 * loadexec = _sceKernelFindModuleByName("sceLoadExec");

	//SysMemForKernel_458A70B5
	int (* _sceKernelGetModel)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelGetModel);

	psp_model = _sceKernelGetModel();

	if(psp_model == PSP_GO) {
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}

	//replace LoadReboot function
	_sw(MAKE_CALL(_LoadReboot), loadexec->text_addr + patch->LoadRebootCall);

	//patch Rebootex position to 0x88FC0000
	_sw(0x3C0188FC, loadexec->text_addr + patch->RebootJump); // lui $at, 0x88FC

	//save LoadReboot function
	LoadReboot = (void*)loadexec->text_addr + patch->LoadReboot;

	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//return success
	return 0xC01DB15D;
}

int install_in_cfw(void)
{
	//installer load result
	int result = 0;

	//load installer module
	SceUID mod = sceKernelLoadModule(installerpath, 0, NULL);

	//installer loaded
	if (mod >= 0) {
		//start installer
		result = sceKernelStartModule(mod, strlen(installerpath) + 1, installerpath, NULL, NULL);
	}

	return 0;
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

int gzip_decompress(u8 *dst, const u8 *src, int size)
{
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = inflateInit2(&strm, 15+16);

	if(ret != Z_OK) {
		return -1;
	}

	strm.avail_in = size;
	strm.next_in = (void*)src;
	strm.avail_out = 1024*1024L;
	strm.next_out = decompress_buf;

	ret = inflate(&strm, Z_FINISH);

	if(ret == Z_STREAM_ERROR) {
		inflateEnd(&strm);

		return -3;
	}

	memcpy(dst, decompress_buf, strm.total_out);
	deflateEnd(&strm);

	return strm.total_out;
}

static u8 file_buffer[1024 * 1024L];

void write_files(const char *base)
{
	char fn[256];
	int newsize;

	newsize = gzip_decompress(file_buffer, installer, size_installer);

	if(newsize < 0) {
		pspDebugScreenPrintf("cannot decompress installer %d\n", newsize);

		return;
	}

	strcpy(fn, base);
	strcat(fn, "installer.prx");
	write_file(fn, file_buffer, newsize);

	newsize = gzip_decompress(file_buffer, Rebootex_prx, size_Rebootex_prx);
	
	if(newsize < 0) {
		pspDebugScreenPrintf("cannot decompress rebootex %d\n", newsize);
		
		return;
	}

	strcpy(fn, base);
	strcat(fn, "Rebootex.prx");
	write_file(fn, file_buffer, newsize);
}

//entry point
int main(int argc, char * argv[])
{
	pspDebugScreenInit();

	psp_fw_version = sceKernelDevkitVersion();

	setup_patch_offset_table(psp_fw_version);
	
	//puzzle installer path
	strcpy(installerpath, argv[0]);

	char * slash = strrchr(installerpath, '/');
	if (slash) slash[1] = '\0';
	
	write_files(installerpath);
	strcat(installerpath, "installer.prx");

	printk_init("ms0:/launcher.txt");
	printk("Hello exploit\n");

	if(sctrlHENGetVersion() >= 0) {
		install_in_cfw();

		return 0;
	}

	do_exploit_660();

	//trigger reboot
	sceKernelExitGame();

	//kill thread
	sceKernelExitDeleteThread(0);

	//return
	return 0;
}

//exit point
int module_stop(SceSize args, void * argp)
{
	//return
	return 0;
}
