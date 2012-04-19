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
#include "galaxy_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.StoreFd = 0x00000188 + 0x00008900, /* See 0x00004D98 */
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data5 = 0x00000114 + 0x00008900, /* See 0x000033B0 */
	.InitForKernelCall = 0x00003C5C,
	.Func1 = 0x00003C78,
	.Func2 = 0x00004414,
	.Func3 = 0x0000596C,
	.Func4 = 0x000036A8,
	.Func5 = 0x00004FEC,
	.Func6 = 0x0000505C,
	.sceIoClose = 0x00007D68,
	.sceKernelCreateThread = 0x00019264,
	.sceKernelStartThread = 0x00019408,
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	g_offs = &g_660_offsets;
}
