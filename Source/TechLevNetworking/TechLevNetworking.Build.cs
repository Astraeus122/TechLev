// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TechLevNetworking : ModuleRules
{
    public TechLevNetworking(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
    }
}
