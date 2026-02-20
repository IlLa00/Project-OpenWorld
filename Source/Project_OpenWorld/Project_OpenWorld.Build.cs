using UnrealBuildTool;

public class Project_OpenWorld : ModuleRules
{
	public Project_OpenWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] {
            ModuleDirectory
        });

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"Niagara", "UMG",
			"WorldPartitionEditor",
			"PCG",
			"AIModule",
            "StateTreeModule",
			"GameplayStateTreeModule",
			"StructUtils",
            "HTTP",
			"Json",
			"JsonUtilities",
			"NavigationSystem",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"RenderCore", "Renderer", "Slate", "SlateCore",
		});
	}
}
