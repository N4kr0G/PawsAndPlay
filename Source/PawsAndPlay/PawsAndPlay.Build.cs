using UnrealBuildTool;

public class PawsAndPlay : ModuleRules
{
	public PawsAndPlay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore",
			"UMG", "Slate", "SlateCore", "HTTP", "Json"
		});
	}
}
