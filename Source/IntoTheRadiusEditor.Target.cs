using UnrealBuildTool;

public class IntoTheRadiusEditorTarget : TargetRules {
	public IntoTheRadiusEditorTarget(TargetInfo Target) : base(Target) {
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[] {
			"IntoTheRadius",
			"ITR_MobileShaders",
			"OnlineSubsystemOculus",
		});
	}
}
