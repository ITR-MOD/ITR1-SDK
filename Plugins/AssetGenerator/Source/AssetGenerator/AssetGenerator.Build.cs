using UnrealBuildTool;

public class AssetGenerator : ModuleRules
{
    public AssetGenerator(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;
        bUseUnity = false;

        bLegacyPublicIncludePaths = false;
        ShadowVariableWarningLevel = WarningLevel.Off;
        
        PublicDependencyModuleNames.AddRange(new[] {
            "Core", 
            "CoreUObject",
            "Engine",
            "InputCore",
            "BlueprintGraph",
            "KismetCompiler",
            "AssetRegistry",
            "SlateCore", "Slate", "UMG",
            "Projects",
            "Landscape",
            "Json",
			"UnrealEd",
            "UMGEditor",
			"BlueprintGraph",
            "MovieSceneTracks",
            "AssetDumper",
            "EditorStyle"
        });
        
		PrivateDependencyModuleNames.AddRange(new[] {
            "DesktopPlatform",
            "AIModule",
            "MaterialEditor",
            "RHI", 
            "AnimGraph",
            "AnimGraphRuntime",
            "ControlRig",
            "Niagara",
            "NiagaraEditor",
            "ControlRigDeveloper",
            "ControlRigEditor",
            "RigVM",
            "JsonUtilities",
            "Kismet",
            "WorkspaceMenuStructure",
            "PhysicsCore",
            "MediaAssets",
            "AudioEditor",
            "AudioMixer",
            "Synthesis",
            "ResonanceAudio",
            "GraphEditor"
        });
        PrivateDependencyModuleNames.Add("ScriptDisassembler");
        PrivateDependencyModuleNames.Add("SourceControl");

	    PrivateDependencyModuleNames.Add("DeveloperSettings");
    }
}
