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

#ifndef REBOOTEX_BIN_PATCH_OFFSET_H
#define REBOOTEX_BIN_PATCH_OFFSET_H

#include <pspsdk.h>
#include "utils.h"

struct RebootexPatch {
	u32 sceBootLfatOpen;
	u32 sceBootLfatRead;
	u32 sceBootLfatClose;
	u32 UnpackBootConfig;
	u32 sceBootLfatOpenCall;
	u32 sceBootLfatReadCall;
	u32 sceBootLfatCloseCall;
	u32 UnpackBootConfigCall;
	u32 RebootexCheck1;
	u32 RebootexCheck2;
	u32 RebootexCheck3;
	u32 RebootexCheck4;
	u32 RebootexCheck5;
	u32 LoadCoreModuleStartCall;
	u32 UnpackBootConfigBufferAddress;
};

struct LoadCorePatch {
	u32 DecryptPSP;
	u32 sceKernelCheckExecFile;
	u32 DecryptPSPCall1;
	u32 DecryptPSPCall2;
	u32 sceKernelCheckExecFileCall1;
	u32 sceKernelCheckExecFileCall2;
	u32 sceKernelCheckExecFileCall3;
};

typedef struct _PatchOffset {
	u32 fw_version;
	u32 iCacheFlushAll;
	u32 dCacheFlushAll;
	struct RebootexPatch rebootex_patch_01g;
	struct RebootexPatch rebootex_patch_other;
	struct LoadCorePatch loadcore_patch;
} PatchOffset;

extern PatchOffset *g_offs;

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.iCacheFlushAll = 0x0000013C,
	.dCacheFlushAll = 0x00000890,
	.rebootex_patch_01g = {
		.sceBootLfatOpen = 0x0000822C,
		.sceBootLfatRead = 0x000083A0,
		.sceBootLfatClose = 0x00008344,
		.UnpackBootConfig = 0x0000574C,
		.sceBootLfatOpenCall = 0x000027C4,
		.sceBootLfatReadCall = 0x00002834,
		.sceBootLfatCloseCall = 0x00002860,
		.UnpackBootConfigCall = 0x000070F8,
		.RebootexCheck1 = 0x00003880,
		.RebootexCheck2 = 0x000027BC,
		.RebootexCheck3 = 0x00002810,
		.RebootexCheck4 = 0x00002828,
		.RebootexCheck5 = 0x00007390,
		.LoadCoreModuleStartCall = 0x00005644,
		.UnpackBootConfigBufferAddress = 0x000070D4,
	},
	.rebootex_patch_other = {
		.sceBootLfatOpen = 0x000082EC,
		.sceBootLfatRead = 0x00008460,
		.sceBootLfatClose = 0x00008404,
		.UnpackBootConfig = 0x0000580C,
		.sceBootLfatOpenCall = 0x0000288C,
		.sceBootLfatReadCall = 0x000028FC,
		.sceBootLfatCloseCall = 0x00002928,
		.UnpackBootConfigCall = 0x000071B8,
		.RebootexCheck1 = 0x00003948,
		.RebootexCheck2 = 0x00002884,
		.RebootexCheck3 = 0x000028D8,
		.RebootexCheck4 = 0x000028F0,
		.RebootexCheck5 = 0x00007450,
		.LoadCoreModuleStartCall = 0x00005704,
		.UnpackBootConfigBufferAddress = 0x00007194,
	},
	.loadcore_patch = {
		.DecryptPSP = 0x0000783C - 0x00000AF8,
		.sceKernelCheckExecFile = 0x00007824 - 0x00000AF8,
		.DecryptPSPCall1 = 0x00003E70 - 0x00000AF8,
		.DecryptPSPCall2 = 0x00005970 - 0x00000AF8,
		.sceKernelCheckExecFileCall1 = 0x00005994 - 0x00000AF8,
		.sceKernelCheckExecFileCall2 = 0x000059C4 - 0x00000AF8,
		.sceKernelCheckExecFileCall3 = 0x00005A5C - 0x00000AF8,
	},
};

PatchOffset *g_offs = NULL;

static inline void setup_patch_offset_table(u32 fw_version)
{
	g_offs = &g_660_offsets;
}

#endif
