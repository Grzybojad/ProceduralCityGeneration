// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProcGen : ModuleRules
{
	public ProcGen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "ProceduralMeshComponent", "Voronoi" });
	}
}
