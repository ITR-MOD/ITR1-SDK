// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Alpakit : ModuleRules
{
	public Alpakit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bLegacyPublicIncludePaths = false;
        CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new[] {
			"Core",
			"DesktopPlatform",
		});
		
		PrivateDependencyModuleNames.AddRange(new[] {
			"ApplicationCore",
			"Json",
			"Projects",
			"InputCore",
			"UnrealEd",
			"LevelEditor",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"ExternalImagePicker",
			"EditorStyle",
			"PluginBrowser",
			"LauncherServices",
			"TargetDeviceServices", 
			"UATHelper", "ToolMenus", "PropertyEditor", "SourceControl", "AssetTools", "AssetRegistry", "Settings", "JsonUtilities",
		});
	}
}
