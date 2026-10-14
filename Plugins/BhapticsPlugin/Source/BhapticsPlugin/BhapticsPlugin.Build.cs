//Copyright bHaptics Inc. All Rights Reserved 2022~

using UnrealBuildTool;

public class BhapticsPlugin : ModuleRules
{
	public BhapticsPlugin(ReadOnlyTargetRules Target) : base(Target)
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
                "Projects"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Json",
                "JsonUtilities",
                "Sockets", 
                "Networking", 
                "Json", 
                "HTTP",
				"BhapticsCPP",
                "BhapticsAndroid",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
