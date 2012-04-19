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
#include "stargate_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.sceMesgLedDecryptGame1 = {
		0x00003160, // 01g
		0x000034F8, // 02g
		0x00003804, // 03g
		0x00003804, // 04g
		0x00003B28, // 05g
		0xDEADBEEF, // 06g
		0x00003804, // 07g
		0xDEADBEEF, // 08g
		0x00003804, // 09g
		0xDEADBEEF, // unused
		0x00003804, // 11G
	},
	.sceMesgLedDecryptGame2 = {
		0x000033F8, // 01g
		0x000037D8, // 02g
		0x00003B78, // 03g
		0x00003B78, // 04g
		0x00003EE4, // 05g
		0xDEADBEEF, // 06g
		0x00003B78, // 07g
		0xDEADBEEF, // 08g
		0x00003B78, // 09g
		0xDEADBEEF, // unused
		0x00003B78, // 11G
	},
	.mesgled_decrypt = 0x000000E0,
	.LoadExecForUser_362A956B_fix = 0x000009B4,
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	g_offs = &g_660_offsets;
}
