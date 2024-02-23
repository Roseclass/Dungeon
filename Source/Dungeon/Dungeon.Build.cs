// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dungeon : ModuleRules
{
	public Dungeon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"SlateCore",
			"HeadMountedDisplay",
			"NavigationSystem",
			"GameplayTasks",
			"AIModule",
			"UMG",
			"ProceduralMeshComponent",
			"Niagara",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PublicIncludePaths.Add(ModuleDirectory);
	}
}
