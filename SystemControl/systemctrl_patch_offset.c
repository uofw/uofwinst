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
#include "systemctrl_patch_offset.h"

PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.interruptman_patch = {
		.InvalidSyscallCheck1 = 0x00000DEC,
		.InvalidSyscallCheck2 = 0x00000E98,
	},
	.modulemgr_patch = {
		.sctrlKernelSetUMDEmuFile = 0x000099A0 + 0x00000008,   /* See 0x00005C1C */
		.sctrlKernelSetInitFileName = 0x000099A0 + 0x00000004, /* See 0x00004F48 */
		.ProbeExec3 = 0x00008824,
		.ProbeExec3Call = 0x00007C5C,
		.sceKernelCheckExecFileImport = 0x00008884,
		.PartitionCheck = 0x00007FD0,
		.PartitionCheckCall1 = 0x0000651C,
		.PartitionCheckCall2 = 0x00006898,
		.DeviceCheck1 = 0x00000760,
		.DeviceCheck2 = 0x000007C0,
		.DeviceCheck3 = 0x000030B0,
		.DeviceCheck4 = 0x0000310C,
		.DeviceCheck5 = 0x00003138,
		.DeviceCheck6 = 0x00003444,
		.DeviceCheck7 = 0x0000349C,
		.DeviceCheck8 = 0x000034C8,
		.PrologueModule = 0x00008124,
		.PrologueModuleCall = 0x00007048,
		.StartModule = 0x00006FF4,
		.StartModuleCall = 0x00000290,
		.sceKernelLinkLibraryEntriesCall = 0x0000843C,
		.sceKernelLinkLibraryEntriesForUserCall = 0x00008188,
	},
	.threadmgr_patch = {
		.sctrlKernelSetUserLevel = 0x00019F40,
	},
	.mediasync_patch = {
		.sceSystemFileGetIndex = 0x00000F40,
		.MsCheckMediaCheck = 0x00000744,
		.DiscCheckMediaCheck1 = 0x000003C4,
		.DiscCheckMediaCheck2 = 0x00000DC8,
		.MsSystemFileCheck = 0x000010B4,
		.DiscIDCheck1 = 0x00000FC0,
		.DiscIDCheck2 = 0x00000FDC,
		.sceSystemFileGetIndexCall = 0x0000097C,
	},
	.memlmd_patch_01g = {
		.memlmd_TagPatch = 0x000013F0,
		.memlmd_unsigner = 0x00001070,
		.memlmd_decrypt = 0x0000020C,
		.sceMemlmdInitializeScrambleKey = 0x000012B8,
		.memlmd_unsigner_call1 = 0x00001238,
		.memlmd_unsigner_call2 = 0x0000128C,
		.memlmd_decrypt_call1 = 0x00000F70,
		.memlmd_decrypt_call2 = 0x00000FD4,
	},
	.memlmd_patch_other = {
		.memlmd_TagPatch = 0x000014B0,
		.memlmd_unsigner = 0x000010F8,
		.memlmd_decrypt = 0x0000020C,
		.sceMemlmdInitializeScrambleKey = 0x00001340,
		.memlmd_unsigner_call1 = 0x000012C0,
		.memlmd_unsigner_call2 = 0x00001314,
		.memlmd_decrypt_call1 = 0x00000FF8,
		.memlmd_decrypt_call2 = 0x0000105C,
	},
	.mesgled_patch = {
		.mesg_decrypt_call = {
			/*0x457B0CF0  0xE71C010B  0xADF305F0  0x457B0AF0  0x380280F0 */
			{ 0x00003160, 0x00003FD8, 0x00004DD8, 0x00002354, 0x00002BE0 }, // 01g
			{ 0x000034F8, 0x00004548, 0x000047EC, 0x0000475C, 0x00001F64 }, // 02g
			{ 0x00003850, 0x00004A70, 0x00004D5C, 0x000038E0, 0x00001FAC }, // 03g
			{ 0x00003850, 0x00004A70, 0x00004D5C, 0x000038E0, 0x00001FAC }, // 04g
			{ 0x000024E4, 0x00004F44, 0x00002404, 0x00002694, 0x00002F1C }, // 05g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 06g
			{ 0x00003850, 0x00004A70, 0x00004D5C, 0x000038E0, 0x00001FAC }, // 07g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 08g
			{ 0x00003850, 0x00004A70, 0x00004D5C, 0x000038E0, 0x00001FAC }, // 09g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 10g
			{ 0x00003850, 0x00004A70, 0x00004D5C, 0x000038E0, 0x00001FAC }, // 11g
		},
		.mesgled_decrypt = 0x000000E0,
	},
	.sysmem_patch = {
		.sctrlKernelSetDevkitVersion = 0x0001191C,
		.sysmemforuser_patch = {
			{ 0x000098F0, 0x1F }, // sceKernelSetCompiledSdkVersion
			{ 0x00009A10, 0x12 }, // SysMemUserForUser_342061E5
			{ 0x00009AA8, 0x18 }, // SysMemUserForUser_315AD3A0
			{ 0x00009B58, 0x1C }, // SysMemUserForUser_EBD5C3E6
			{ 0x00009C2C, 0x15 }, // SysMemUserForUser_057E7380
			{ 0x00009CD0, 0x15 }, // SysMemUserForUser_91DE343C
			{ 0x00009D74, 0x12 }, // SysMemUserForUser_7893F79A
			{ 0x00009E0C, 0x18 }, // SysMemUserForUser_35669D4C
			{ 0x00009EBC, 0x18 }, // SysMemUserForUser_1B4217BC
			{ 0x00009F6C, 0x12 }, // SysMemUserForUser_358CA1BB
		},
		.get_partition = 0x00003D28,
	},
	.iofilemgr_patch = {
		.sctrlHENFindDriver = 0x00002A4C,
	},
	.march33_patch = {
		.MsMediaInsertedCheck = 0x000009E8,
	},
	.psnfix_patch = {
		.NPSignupImageVersion = 0x00038CBC,
		.NPSigninCheck = 0x00006CF4,
		.NPSigninImageVersion = 0x000096C4,
		.NPMatchVersionMajor = 0x00004604,
		.NPMatchVersionMinor = 0x0000460C,
	},
	.wlan_driver_patch = {
		.FreqCheck = 0x000026C0,
	},
	.power_service_patch = {
		.scePowerGetBacklightMaximumCheck = 0x00000E68,
	},
	.syscon_patch = {
		.sceSysconPowerStandby = 0x00002D08,
	},
	.init_patch = {
		.sceKernelStartModuleImport = 0x00001C3C,
		.module_bootstart = 0x00001A4C,
	},
	.umdcache_patch = {
		.module_start = 0x000009C8,
	},
	.loadercore_patch = {
		.sceKernelCheckExecFilePtr = 0x00007B5C,
		.sceKernelCheckExecFileCall1 = 0x000011F0,
		.sceKernelCheckExecFileCall2 = 0x00001240,
		.sceKernelCheckExecFileCall3 = 0x000048B4,
		.ReloactionTable = 0x00007F94,
		.ProbeExec1 = 0x00006468,
		.ProbeExec1Call = 0x000044B0,
		.ProbeExec2 = 0x000063C0,
		.ProbeExec2Call1 = 0x000046B0,
		.ProbeExec2Call2 = 0x000066D4,
		.EnableSyscallExport = 0x00003D70,
		.LoaderCoreCheck1 = 0x000073A4,
		.LoaderCoreCheck2 = 0x00005900,
		.LoaderCoreCheck3 = 0x00005A10,
		.LoaderCoreUndo1Call1 = 0x00005994,
		.LoaderCoreUndo1Call2 = 0x000059C4,
		.LoaderCoreUndo1Call3 = 0x00005A5C,
		.LoaderCoreUndo2Call1 = 0x00003E70,
		.LoaderCoreUndo2Call2 = 0x00005970,
		.memlmd_323366CA_NID = 0x6192F715,
		.memlmd_7CF1CD3E_NID = 0xEF73E85B,
		.pops_version_check = 0x00006BA4,
		.sceInitBootStartCall = 0x00001A28,
		.sceKernelLinkLibraryEntries = 0x00001110,
		.sceKernelLinkLibraryEntriesForUser = 0x000025A4,
		.sceKernelIcacheClearAll = 0x0000748C,
	},
	.loadexec_patch_other = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D5C,
		.RebootJump = 0x00002DA8,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x000023D0,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002414,
		.sceKernelExitVSHVSHCheck1 = 0x000016A4,
		.sceKernelExitVSHVSHCheck2 = 0x000016D8,
		.sctrlKernelLoadExecVSHWithApitype = 0x00002384,
	},
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002FA8,
		.RebootJump = 0x00002FF4,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x00002624,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002668,
		.sceKernelExitVSHVSHCheck1 = 0x000016A4,
		.sceKernelExitVSHVSHCheck2 = 0x000016D8,
		.sctrlKernelLoadExecVSHWithApitype = 0x000025D8,
	},
	.impose_patch = {
		2,
		0x00000574,
	},
	.usb_patch = {
		.scePowerBatteryDisableUsbChargingStub = 0x00008FE8,
		.scePowerBatteryEnableUsbChargingStub = 0x00008FF0,
	},
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version __attribute__((unused)))
{
	g_offs = &g_660_offsets;
}
