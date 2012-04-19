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
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "systemctrl.h"
#include "utils.h"
#include "../Rebootex_bin/rebootex.h"
#include "../PXE/Launcher/launcher_patch_offset.h"
#include "rebootex_conf.h"
#include "../CrossFW/cross_fw.h"

u32 psp_model;
u32 psp_fw_version;

PSP_MODULE_INFO("fast_recovery", 0x1000, 1, 0);

extern u32 sceKernelGetModel_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern SceModule* sceKernelFindModuleByName_620(char *modname);

extern u32 sceKernelGetModel_660(void);
extern u32 sceKernelDevkitVersion_660(void);
extern SceModule* sceKernelFindModuleByName_660(char *modname);

static int (*LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

rebootex_args *g_argp;
SceSize g_args;

void build_rebootex_configure(u8 patch_memlmd)
{
	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG);
	
	conf->magic = REBOOTEX_CONFIG_MAGIC;
	conf->psp_fw_version = psp_fw_version;
	conf->psp_model = psp_model;
	conf->patch_memlmd = patch_memlmd;
}

//load reboot wrapper
int load_reboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((void*)REBOOTEX_START, rebootex, size_rebootex);

	//reset reboot flags
	memset((void*)REBOOTEX_CONFIG, 0, 0x100);
	memset((void*)REBOOTEX_CONFIG_ISO_PATH, 0, 256);

    u8 patch_memlmd = 1;
    if (g_args > 4)
    {
        int i;
        for (i = 0; i < 256; i++)
        	memset(REBOOTEX_CONFIG_MODULE(i), 0, 256);
    	for (i = 0; i < g_argp->modcount; i++)
    	{
    	    strncpy(REBOOTEX_CONFIG_MODULE(i), g_argp->modname[i], 256);
    	    if (patch_memlmd && strncmp(g_argp->modname[i], "memlmd", sizeof("memlmd")) == 0)
    	        patch_memlmd = 0;
    	}
    }

	build_rebootex_configure(patch_memlmd);

	//forward
	return (*LoadReboot)(arg1, arg2, arg3, arg4);
}

// mode: 0 - OFW 1 - CFW
void patch_sceLoadExec(int mode)
{
	SceModule2 * loadexec = (SceModule2*)sctrlKernelFindModuleByName("sceLoadExec");
	struct sceLoadExecPatch *patch;

	if (loadexec == NULL) {
		return;
	}

	if(psp_model == PSP_GO) { // PSP-N1000
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}
	
	//save LoadReboot function
	LoadReboot = (void*)loadexec->text_addr;

	if(mode == 0) {
		//restore LoadReboot
		_sw(MAKE_CALL(LoadReboot), loadexec->text_addr + patch->LoadRebootCall);

		//restore jmp to 0x88600000
		_sw(0x3C018860, loadexec->text_addr + patch->RebootJump);
	} else if(mode == 1) {
		//replace LoadReboot function
		_sw(MAKE_CALL(load_reboot), loadexec->text_addr + patch->LoadRebootCall);

		//patch Rebootex position to 0x88FC0000
		_sw(0x3C0188FC, loadexec->text_addr + patch->RebootJump); // lui $at, 0x88FC
	}

	sync_cache();
}

int main_thread(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	int mode = 1;
	if(g_args >= 4)
		mode = g_argp->mode;

	patch_sceLoadExec(mode);
	sctrlKernelExitVSH(NULL);

	return 0;
}

int module_start(SceSize args, void* argp)
{
	int thid;

	psp_fw_version = sctrlKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);
	psp_model = sctrlKernelGetModel();
	g_args = args;
	g_argp = argp;

	thid = sceKernelCreateThread("fastRecovery", main_thread, 0x1A, 0x1000, 0, NULL);

	if(thid>=0) {
		sceKernelStartThread(thid, 0, NULL);
	}

	return 0;
}

int module_stop(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	return 0;
}
