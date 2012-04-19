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
#include "launcher_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.sysmem_patch = {
		.sceKernelIcacheInvalidateAll = 0x00000E98,
		.sceKernelDcacheWritebackInvalidateAll = 0x00000744,
		.sceKernelGetModel = 0x0000A0B0,
		.sceKernelPowerLockForUser = 0x0000CBB8,
		.sceKernelPowerLockForUser_data_offset = 0x000040F8,
	},
	.sceKernelFindModuleByName = 0x88017000 + 0x00006F98,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002FA8,
		.RebootJump = 0x00002FF4,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D5C,
		.RebootJump = 0x00002DA8,
	},
	.patchRangeStart = 0xDEADBEEF,
	.patchRangeEnd = 0xDEADBEEF,
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version __attribute__((unused)))
{
	g_offs = &g_660_offsets;
}

