using UnrealBuildTool;

public class TechLevNetworking : ModuleRules
{
    public TechLevNetworking(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
            "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystem",
            "OnlineSubsystemUtils", "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");

        // Conditional addition of editor modules
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UMGEditor",
                "UnrealEd",
                "Sequencer"
            });
        }
    }
}
