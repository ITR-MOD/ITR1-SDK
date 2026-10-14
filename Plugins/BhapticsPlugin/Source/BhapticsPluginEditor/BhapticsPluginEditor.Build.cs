//Copyright bHaptics Inc. All Rights Reserved 2022~

using UnrealBuildTool;

public class BhapticsPluginEditor : ModuleRules
{
	public BhapticsPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
				"EditorStyle",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "Json",
                "JsonUtilities",
                "BhapticsCPP",
                "BhapticsAndroid",
                "BhapticsPlugin"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
