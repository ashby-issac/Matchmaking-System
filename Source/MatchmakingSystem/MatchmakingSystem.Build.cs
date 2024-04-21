// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MatchmakingSystem : ModuleRules
{
	public MatchmakingSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystem", "OnlineSubsystemSteam" });
	}
}
