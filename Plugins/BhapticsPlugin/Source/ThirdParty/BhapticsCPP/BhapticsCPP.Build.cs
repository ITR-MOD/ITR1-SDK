//Copyright bHaptics Inc. All Rights Reserved 2022~

using System.IO;
using UnrealBuildTool;

public class BhapticsCPP : ModuleRules
{
	public BhapticsCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Include path to the public header files
			PublicSystemIncludePaths.Add("$(ModuleDir)/Public");
            
			// Add the library to the additional libraries path
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "bhaptics_library.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("bhaptics_library.dll");

			// Ensure that the DLL is staged along with the executable
			// Adjust the path as necessary based on the plugin's location
			string runtimeDependencyPath;
			if (IsProjectPlugin(Target))
			{
				runtimeDependencyPath = Path.Combine("$(ProjectDir)", "Plugins", "BhapticsPlugin", "Source", "ThirdParty", "BhapticsCPP", "Win64", "bhaptics_library.dll");
			}
			else
			{
				runtimeDependencyPath = "$(PluginDir)/Source/ThirdParty/BhapticsCPP/Win64/bhaptics_library.dll";
			}
			RuntimeDependencies.Add(runtimeDependencyPath);
		}
	}
	private bool IsProjectPlugin(ReadOnlyTargetRules Target)
	{
		// Get the full path to the project directory
		string projectDirectory = Path.GetDirectoryName(Target.ProjectFile.ToString());

		// Normalize paths to ensure consistency in separators and case
		string normalizedPluginBaseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../")).Replace("\\", "/");
		string normalizedProjectDirectory = projectDirectory.Replace("\\", "/").ToLower();

		// A project plugin's path will include the project directory's path
		return normalizedPluginBaseDirectory.ToLower().StartsWith(normalizedProjectDirectory);
	}
}