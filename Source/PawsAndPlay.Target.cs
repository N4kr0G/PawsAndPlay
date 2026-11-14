using UnrealBuildTool;

public class PawsAndPlayTarget : TargetRules
{
	public PawsAndPlayTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("PawsAndPlay");
	}
}
