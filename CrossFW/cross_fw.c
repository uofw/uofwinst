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

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspsysmem_kernel.h>
#include <pspcrypt.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "cross_fw.h"

extern u32 psp_fw_version;
extern u32 psp_model;

extern u32 sceKernelGetModel_660(void);
extern u32 sceKernelDevkitVersion_660(void);
extern SceModule* sceKernelFindModuleByName_660(char *modname);
extern int sceKernelExitVSH_660(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDisc_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDiscUpdater_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs1_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs2_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs4_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelSetDdrMemoryProtection_660(void *addr, int size, int prot);
extern SceUID sceKernelCreateHeap_660(SceUID partitionid, SceSize size, int unk, const char *name);
extern int sceKernelDeleteHeap_660(SceUID heapid);
extern int sceKernelFreeHeapMemory_660(SceUID heapid, void *block);
extern void* sceKernelAllocHeapMemory_660(SceUID heapid, SceSize size);
extern int sceKernelGetSystemStatus_660(void);
extern int sceKernelQueryMemoryPartitionInfo_660(int pid, PspSysmemPartitionInfo *info);
extern int sceKernelPartitionMaxFreeMemSize_660(int pid);
extern int sceKernelPartitionTotalFreeMemSize_660(int pid);
extern u32 sceKernelQuerySystemCall_660(void *func);
extern SceModule* sceKernelFindModuleByUID_660(SceUID modid);
extern SceModule* sceKernelFindModuleByAddress_660(u32 address);
extern int sceKernelCheckExecFile_660(unsigned char * buffer, int * check);	
extern int sceKernelLoadModule_660(const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelStartModule_660(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
extern int sceKernelUnloadModule_660(SceUID modid);
extern SceUID sceKernelLoadModuleWithApitype2_660(int apitype, const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelBootFromGo_660(void);
void* sceKernelGetBlockHeadAddr_660(SceUID blockid);
SceUID sceKernelAllocPartitionMemory_660(SceUID partitionid, const char * name, int type, SceSize size, void * addr);

extern int *InitForKernel_040C934B(void);
extern int sceKernelExitVSH(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelGetSystemStatus(void);
extern u32 sceKernelQuerySystemCall(void *func);
extern int sceKernelCheckExecFile(unsigned char * buffer, int * check);
extern SceUID _sceKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option);
void* sceKernelGetBlockHeadAddr(SceUID blockid);
SceUID sceKernelAllocPartitionMemory(SceUID partitionid, const char * name, int type, SceSize size, void * addr);

extern int sceCtrlReadBufferPositive_660(SceCtrlData *pad_data, int count);

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelExitVSH_660(param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHDisc_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHDiscUpdater_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs1_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs2_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHEf2_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs3_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs4_660(file, param);
	pspSdkSetK1(k1);

	return ret;
}

SceUID sctrlKernelAllocPartitionMemory(SceUID partitionid, const char * name, int type, SceSize size, void * addr)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelAllocPartitionMemory_660(partitionid, name, type, size, addr);
	pspSdkSetK1(k1);

	return ret;
}

void* sctrlKernelGetBlockHeadAddr(SceUID blockid)
{
	u32 k1;
	void* ret = NULL;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetBlockHeadAddr_660(blockid);
	pspSdkSetK1(k1);

	return ret;
}

u32 sctrlKernelGetModel(void)
{
	return sceKernelGetModel_660();
}

u32 sctrlKernelDevkitVersion(void)
{
    return sceKernelDevkitVersion_660();
}

SceModule* sctrlKernelFindModuleByName(char *modname)
{
	return sceKernelFindModuleByName_660(modname);
}

int sctrlKernelSetDdrMemoryProtection(void *addr, int size, int prot)
{
	return sceKernelSetDdrMemoryProtection_660(addr, size, prot);
}

SceUID sctrlKernelCreateHeap(SceUID partitionid, SceSize size, int unk, const char *name)
{
	return sceKernelCreateHeap_660(partitionid, size, unk, name);
}

int sctrlKernelDeleteHeap(SceUID heapid)
{
	return sceKernelDeleteHeap_660(heapid);
}

int sctrlKernelFreeHeapMemory(SceUID heapid, void *block)
{
	return sceKernelFreeHeapMemory_660(heapid, block);
}

void* sctrlKernelAllocHeapMemory(SceUID heapid, SceSize size)
{
	return sceKernelAllocHeapMemory_660(heapid, size);
}

int sctrlKernelGetSystemStatus(void)
{
    return sceKernelGetSystemStatus_660();
}

int sctrlKernelQueryMemoryPartitionInfo(int pid, PspSysmemPartitionInfo *info)
{
	return sceKernelQueryMemoryPartitionInfo_660(pid, info);
}

int sctrlKernelPartitionMaxFreeMemSize(int pid)
{
    return sceKernelPartitionMaxFreeMemSize_660(pid);
}

int sctrlKernelPartitionTotalFreeMemSize(int pid)
{
    return sceKernelPartitionTotalFreeMemSize_660(pid);
}

int sctrlKernelQuerySystemCall(void *func_addr)
{
	int ret = -1;
	u32 k1;

	k1 = pspSdkSetK1(0);
	ret = sceKernelQuerySystemCall_660(func_addr);
	pspSdkSetK1(k1);

	return ret;
}

SceModule* sctrlKernelFindModuleByUID(SceUID modid)
{
	return sceKernelFindModuleByUID_660(modid);
}

SceModule* sctrlKernelFindModuleByAddress(u32 address)
{
	return sceKernelFindModuleByAddress_660(address);
}

int sctrlKernelCheckExecFile(unsigned char * buffer, int * check)
{
	return sceKernelCheckExecFile_660(buffer, check);
}

int sctrlKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	return sceKernelLoadModule_660(path, flags, option);
}

int sctrlKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	return sceKernelStartModule_660(modid, argsize, argp, status, option);
}

int sctrlKernelUnloadModule(SceUID modid)
{
	return sceKernelUnloadModule_660(modid);
}

SceUID sctrlKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option)
{
	return sceKernelLoadModuleWithApitype2_660(apitype, path, flags, option);
}

int sctrlKernelBootFrom(void)
{
	if(psp_model == PSP_GO)
		return sceKernelBootFromGo_660();
	return sceKernelBootFrom();
}

int sctrlReadBufferPositive(SceCtrlData *pad_data, int count)
{
	return sceCtrlReadBufferPositive_660(pad_data, count);
}

