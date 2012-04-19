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
#include "vshctrl_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.vshbridge_patch = {
		.sceDisplaySetHoldMode = 0x00005630,
		.sceDisplaySetHoldModeCall = 0x00001A34,
		.HibBlockCheck = 0x000051C8,
		.sceCtrlReadBufferPositiveNID = 0xBE30CED0,
	},
	.sysconf_plugin_patch = {
		.SystemVersionStr = 0x0002A62C,
		.SystemVersionMessage = "6.60 PRO-%c",
		.SystemVersion = 0x000192E0,
		.MacAddressStr = 0x0002E9A0,
		.SlimColor = 0x000076EC,
	},
	.game_plugin_patch = {
		.HomebrewCheck = 0x00020528,
		.PopsCheck = 0x00020E6C,
		.MultiDiscPopsCheck = 0x00014850,
		.HidePicCheck1 = 0x0001D858,
		.HidePicCheck2 = 0x0001D864,
		.SkipGameBootSubroute = 0x000194B0,
		.SkipGameBoot = 0x00019130,
		.RifFileCheck = 0x0002062C,
		.RifCompareCheck = 0x00020654,
		.RifTypeCheck = 0x00020668,
		.RifNpDRMCheck = 0x000206D0,
	},
	.htmlviewer_plugin_patch = {
		.htmlviewer_save_location = 0x0001C7FC,
	},
	.msvideo_main_plugin_patch = {
		.checks = {
			0x0003AF24,
			0x0003AFAC,
			0x0003D7EC,
			0x0003DA48,
			0x000441A0,
			0x000745A0,
			0x00088BF0,
			0x0003D764,
			0x0003D7AC,
			0x00043248,
		},
	},
	.vsh_module_patch = {
		.checks = {
			0x000122B0,
			0x00012058,
			0x00012060,
		},
		.loadexecNID1 = 0x21D4D038,
		.loadexecNID2 = 0xE533E98C,
		.loadexecDisc = 0x63E69956,
		.loadexecDiscUpdater = 0x81682A40,
		.PBPFWCheck = {
			0x000119C0,
			0x000121A4,
			0x00012BA4,
			0x00013288,
		},
		.vshbridge_get_model_call = {
			0x0000670C,
			0x0002068C,
			0x0002D240,
		},
	},
	.update_plugin_patch = {
		.UpdatePluginImageVersion1 = 0x000082A4,
		.UpdatePluginImageVersion2 = 0x000082AC,
		.UpdatePluginImageVersion3 = 0x000082A0,
	},
	.SceUpdateDL_library_patch = {
		.SceUpdateDL_UpdateListStr = 0x000032BC,
		.SceUpdateDL_UpdateListCall1 = 0x00002044,
		.SceUpdateDL_UpdateListCall2 = 0x00002054,
		.SceUpdateDL_UpdateListCall3 = 0x00002080,
		.SceUpdateDL_UpdateListCall4 = 0x0000209C,
	},
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	g_offs = &g_660_offsets;
}

