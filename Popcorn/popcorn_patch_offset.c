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
#include "popcorn_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.popsman_patch = {
		.get_rif_path = 0x00000190,
		.get_rif_path_call1 = 0x00002798,
		.get_rif_path_call2 = 0x00002C58,
		.sceNpDrmGetVersionKeyCall = 0x000029C4,
		.scePspNpDrm_driver_9A34AC9F_Call = 0x00002DA8,
		.scePopsManLoadModuleCheck = 0x00001E80,
	},
	.pops_patch = {
		.decomp = {
			{ 0x000D5424, 0x0000DB78 }, // 01G
			{ 0x000D64DC, 0x0000DB78 }, // 02G
			{ 0x000D64DC, 0x0000DB78 }, // 03G
			{ 0x000D654C, 0x0000DBE8 }, // 04G
			{ 0x000D84D8, 0x0000E300 }, // 05G
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0x000D656C, 0x0000DBE8 }, // 07G
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0x000D656C, 0x0000DBE8 }, // 09G
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0x000D5494, 0x0000DBE8 }, // 11G
		},
		.ICON0SizeOffset = {
			0x00036D50, // 01G
			0x00037D8C, // 02G
			0x00037D8C, // 03G
			0x00037E04, // 04G
			0x00039BEC, // 05G
			0xDEADBEEF, // unused
			0x00037E1C, // 07G
			0xDEADBEEF, // unused
			0x00037E1C, // 09G
			0xDEADBEEF, // unused
			0x00036DCC, // 11G
		},
		.manualNameCheck = {
			0x00025248, // 01G
			0x00025754, // 02G
			0x00025754, // 03G
			0x000257CC, // 04G
			0x00026060, // 05G
			0xDEADBEEF, // unused
			0x000257E4, // 07G
			0xDEADBEEF, // unused
			0x000257E4, // 09G
			0xDEADBEEF, // unused
			0x000252C4, // 11G
		},
		.sceMeAudio_67CD7972_NID = 0x2AB4FE43,
	},
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	g_offs = &g_660_offsets;
}

