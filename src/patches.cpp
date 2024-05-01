/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2024 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "mempatch.h"
#include "icvar.h"
#include "irecipientfilter.h"
#include "entity/ccsplayercontroller.h"
#include "entity/ccsplayerpawn.h"
#include "entity/cbasemodelentity.h"
#include "addresses.h"
#include "patches.h"

#include "tier0/memdbgon.h"

extern CGameConfig *g_GameConfig;

CMemPatch g_CommonPatches[] =
{
	CMemPatch("ServerMovementUnlock", "ServerMovementUnlock"),
	CMemPatch("CheckJumpButtonWater", "FixWaterFloorJump"),
	CMemPatch("CategorizeUnderwater", "CategorizeUnderwater"),
	CMemPatch("WaterLevelGravity", "WaterLevelGravity"),
	CMemPatch("CPhysBox_Use", "CPhysBox_Use"),
	CMemPatch("BotNavIgnore", "BotNavIgnore"),
#ifndef _WIN32
	// Linux checks for the nav mesh in each bot_add command, so we patch 3 times
	CMemPatch("BotNavIgnore", "BotNavIgnore"),
	CMemPatch("BotNavIgnore", "BotNavIgnore"),
#endif
};

#ifdef _WIN32
CMemPatch g_ToolsPatches[] =
{
	// Remove some -nocustomermachine checks without needing -nocustomermachine itself
	// as it can break stuff like the asset browser. This is mainly to enable device selection in compiles
	// And yes, it's the exact same signature appearing 4 times, each unhides a certain hammer compile option
	CMemPatch("HammerNoCustomerMachine", "HammerNoCustomerMachine"),
	CMemPatch("HammerNoCustomerMachine", "HammerNoCustomerMachine"),
	CMemPatch("HammerNoCustomerMachine", "HammerNoCustomerMachine"),
	CMemPatch("HammerNoCustomerMachine", "HammerNoCustomerMachine"),
};
#endif

bool InitPatches(CGameConfig *g_GameConfig)
{
	bool success = true;
	for (int i = 0; i < sizeof(g_CommonPatches) / sizeof(*g_CommonPatches); i++)
	{
		if (!g_CommonPatches[i].PerformPatch(g_GameConfig))
			success = false;
	}

#ifdef _WIN32
	// None of the tools are loaded without, well, -tools
	if (CommandLine()->HasParm("-tools"))
		for (int i = 0; i < sizeof(g_ToolsPatches) / sizeof(*g_ToolsPatches); i++)
			g_ToolsPatches[i].PerformPatch(g_GameConfig);
#endif
	return success;
}

void UndoPatches()
{
	for (int i = 0; i < sizeof(g_CommonPatches) / sizeof(*g_CommonPatches); i++)
		g_CommonPatches[i].UndoPatch();

#ifdef _WIN32
	if (CommandLine()->HasParm("-tools"))
	{
		for (int i = 0; i < sizeof(g_ToolsPatches) / sizeof(*g_ToolsPatches); i++)
			g_ToolsPatches[i].UndoPatch();
	}
#endif
}
