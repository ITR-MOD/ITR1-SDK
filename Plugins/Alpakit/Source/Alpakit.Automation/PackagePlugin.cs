using System;
using System.Collections.Generic;
using System.IO.Compression;
using System.Linq;
using System.IO;
using AutomationTool;
using Tools.DotNETCommon;

using AutomationScripts;


namespace Alpakit.Automation {

public class ITR2PackagePlugin : BuildCookRun
{
	public override void ExecuteBuild()
	{
		
		
		var projectParams = SetupParams();
		var descriptor = JsonObject.Read(projectParams.DLCFile);
		if (descriptor.TryGetObjectArrayField("Modules", out var modules) && modules.Length > 0)
			throw new AutomationException("ITR2 packaging supports content-only mods; retail native DLL compatibility is not established.");
		projectParams.PreModifyDeploymentContextCallback = (ProjectParams, SC) =>
		{
			RemapCookedPluginsContentPaths(ProjectParams, SC);
		};
		projectParams.ModifyDeploymentContextCallback = (ProjectParams, SC) =>
		{

			MarkGameFeature(ProjectParams, SC);
		};
		
		try
		{
			DoBuildCookRun(projectParams);
			ArchiveGameplayTags(projectParams);

			var deploymentContexts = new List<DeploymentContext>();
			if (!projectParams.NoClient)
				deploymentContexts.AddRange(Project.CreateDeploymentContext(projectParams, false, false));
			if (projectParams.DedicatedServer)
				deploymentContexts.AddRange(Project.CreateDeploymentContext(projectParams, true, false));

			foreach (var SC in deploymentContexts)
			{
				ArchiveStagedPlugin(projectParams, SC);
			}

			foreach (var SC in deploymentContexts)
			{
                if (ParseParam($"AutoInstallPlugin_{SC.FinalCookPlatform}"))
                {
                    var directory = ParseOptionalStringParam($"PluginInstallDirectory_{SC.FinalCookPlatform}");
                    if (string.IsNullOrWhiteSpace(directory))
                        throw new AutomationException("Plugin install directory is required.");
                    var source = DirectoryReference.Combine(SC.StageDirectory, GetCanonicalModPathRelativeToRoot(projectParams));
                    var destination = DirectoryReference.Combine(new DirectoryReference(Path.GetFullPath(directory)), projectParams.DLCFile.GetFileNameWithoutExtension());
                    if (source == destination || destination.IsUnderDirectory(source) || source.IsUnderDirectory(destination))
                        throw new AutomationException("Plugin install directory overlaps staging: {0}", destination);
                    if (!DirectoryReference.Exists(source))
                        throw new AutomationException("Packaged plugin directory is missing: {0}", source);
                    Directory.CreateDirectory(destination.FullName);
                    foreach (var file in Directory.GetFiles(source.FullName, "*", SearchOption.AllDirectories))
                    {
                        var target = Path.Combine(destination.FullName, Compat.RelativePath(source.FullName, file));
                        Directory.CreateDirectory(Path.GetDirectoryName(target));
                        File.Copy(file, target, true);
                    }
                }
				var copyToGameDirectory = ParseOptionalDirectoryReferenceParam($"CopyToGameDirectory_{SC.FinalCookPlatform}");
				var launchGameType = ParseOptionalEnumParam<LaunchGame.LaunchType>($"LaunchGame_{SC.FinalCookPlatform}");
				var customLaunchPath = ParseOptionalStringParam($"CustomLaunchPath_{SC.FinalCookPlatform}");
				var customLaunchArgs = ParseOptionalStringParam($"CustomLaunchArgs_{SC.FinalCookPlatform}");

				if (copyToGameDirectory != null)
				{
					if (!IsValidGameDir(copyToGameDirectory))
						throw new AutomationException("Invalid game directory specified for CopyToGameDirectory: {0}", copyToGameDirectory);
					DeployStagedPlugin(projectParams, SC, copyToGameDirectory);
				}

				if (launchGameType != null)
					LaunchGame.Launch(launchGameType.Value, customLaunchPath, customLaunchArgs, message => LogInformation(message));
			}
		}
		finally
		{
			if (!projectParams.NoClient)
			{
				var DeployContextList = Project.CreateDeploymentContext(projectParams, false, true);
				foreach (var SC in DeployContextList)
				{
					Project.CleanStagingDirectory(projectParams, SC);
					DeleteDirectory(GetTempDirForStaging(projectParams, SC));
				}
			}

			if (projectParams.DedicatedServer)
			{
				var DeployContextList = Project.CreateDeploymentContext(projectParams, true, true);
				foreach (var SC in DeployContextList)
				{
					Project.CleanStagingDirectory(projectParams, SC);
					DeleteDirectory(GetTempDirForStaging(projectParams, SC));
				}
			}
		}
	}

	private bool IsValidGameDir(DirectoryReference GameDirectory)
	{
		var projectName = ProjectPath.GetFileNameWithoutAnyExtensions();
		var gameModuleDir = DirectoryReference.Combine(GameDirectory, projectName);
		return DirectoryReference.Exists(gameModuleDir);
	}

	protected new ProjectParams SetupParams()
	{
		LogInformation("Setting up ProjectParams for {0}", ProjectPath);

		var Params = new ProjectParams
		(
			Command: this,
			RawProjectPath: ProjectPath,
			
			// Alpakit shared configuration
			ClientCookedTargets: GetClientTargetsToCook(),
			Cook: true,
			AdditionalCookerOptions: "-AllowUncookedAssetReferences",
			DLCIncludeEngineContent: false,
			Pak: true,
			IoStore: false,
			Stage: true,
			UnversionedCookedContent: false,
			
			// TODO @SML: I would like to pass an empty based on release version, but the cooker checks against it
			BasedOnReleaseVersion: ParseParamValue("BasedOnReleaseVersion") ?? "ITRExternalBase"
		);
		
        string baseRegistry = Path.Combine(ProjectPath.Directory.FullName, "Releases", Params.BasedOnReleaseVersion, "WindowsNoEditor", "AssetRegistry.bin");
        if (!File.Exists(baseRegistry) && !File.Exists(Path.Combine(Path.GetDirectoryName(baseRegistry), "Metadata", "DevelopmentAssetRegistry.bin")))
            throw new AutomationException("DLC cooking requires a base release registry: {0}. Set Based On Release Version in Alpakit settings, or use Non-Plugin Mod packaging.", baseRegistry);
		Params.ValidateAndLog();

		return Params;
	}

	private ParamList<string> GetClientTargetsToCook()
	{
		return new ParamList<string>(ProjectPath.GetFileNameWithoutAnyExtensions());
	}

	private static void DeployStagedPlugin(ProjectParams ProjectParams, DeploymentContext SC, DirectoryReference GameDir)
	{
		var fullStagedPluginDirectory = DirectoryReference.Combine(SC.StageDirectory, GetCanonicalModPathRelativeToRoot(ProjectParams));

		var modDir = DirectoryReference.Combine(GameDir, GetCanonicalModPathRelativeToRoot(ProjectParams));

		if (DirectoryReference.Exists(modDir))
			throw new AutomationException("Destination already exists; preserve it before installing another build: {0}", modDir);

		CopyDirectory_NoExceptions(fullStagedPluginDirectory, modDir);
		var tagDirectory = DirectoryReference.Combine(GameDir, ProjectParams.RawProjectPath.GetFileNameWithoutAnyExtensions(), "Config", "Tags");
		foreach (var tag in GetGameplayTagFiles(ProjectParams))
		{
			Directory.CreateDirectory(tagDirectory.FullName);
			File.Copy(tag.Item1, Path.Combine(tagDirectory.FullName, tag.Item2), true);
		}
	}

	private static IEnumerable<Tuple<string, string>> GetGameplayTagFiles(ProjectParams parameters)
	{
		var source = Path.Combine(parameters.DLCFile.Directory.FullName, "Config", "Tags");
		if (!Directory.Exists(source)) yield break;
		var plugin = parameters.DLCFile.GetFileNameWithoutAnyExtensions();
		foreach (var file in Directory.EnumerateFiles(source, "*.ini", SearchOption.AllDirectories).OrderBy(x => x, StringComparer.Ordinal))
		{
			var relative = Compat.RelativePath(source, file).Replace('\\', '_').Replace('/', '_');
			yield return Tuple.Create(file, plugin + "_" + relative);
		}
	}

	private static void ArchiveGameplayTags(ProjectParams parameters)
	{
		var files = GetGameplayTagFiles(parameters).ToArray();
		if (files.Length == 0) return;
		var plugin = parameters.DLCFile.GetFileNameWithoutAnyExtensions();
		var directory = GetArchivedPluginDirectory(parameters);
		Directory.CreateDirectory(directory.FullName);
		var archivePath = Path.Combine(directory.FullName, plugin + "-GameplayTags.zip");
		using var stream = File.Create(archivePath);
		using var archive = new ZipArchive(stream, ZipArchiveMode.Create);
		var project = parameters.RawProjectPath.GetFileNameWithoutAnyExtensions();
		foreach (var file in files)
			archive.CreateEntryFromFile(file.Item1, project + "/Config/Tags/" + file.Item2);
		LogInformation("Gameplay tag startup archive: {0}. Install beside the game project folder before launching.", archivePath);
	}

	private static void ArchiveStagedPlugin(ProjectParams ProjectParams, DeploymentContext SC)
	{
		var archiveDirectory = GetArchivedPluginDirectory(ProjectParams);

		var dlcName = ProjectParams.DLCFile.GetFileNameWithoutAnyExtensions();
		
		var zipFileName = $"{dlcName}.zip";

		var fullStagedPluginDirectory = DirectoryReference.Combine(SC.StageDirectory, GetCanonicalModPathRelativeToRoot(ProjectParams));

		CreateZipFromDirectory(fullStagedPluginDirectory, FileReference.Combine(archiveDirectory, zipFileName));
	}
    
	private static DirectoryReference GetArchivedPluginDirectory(ProjectParams ProjectParams)
	{
		// Plugins will be archived under <Project>/Saved/ArchivedPlugins/<DLCName>
		return DirectoryReference.Combine(ProjectParams.RawProjectPath.Directory, "Saved", "ArchivedPlugins", ProjectParams.DLCFile.GetFileNameWithoutAnyExtensions());
	}
	
	private static void CreateZipFromDirectory(DirectoryReference SourceDirectory, FileReference DestinationFile)
	{
		CreateDirectory(DestinationFile.Directory);
		
		if (FileReference.Exists(DestinationFile))
			FileReference.Delete(DestinationFile);

		ZipFile.CreateFromDirectory(SourceDirectory.FullName, DestinationFile.FullName);
	}

	private static bool IsGameFeatureDLC(ProjectParams ProjectParams)
	{
		var gameFeaturesPluginsRoot = DirectoryReference.Combine(ProjectParams.RawProjectPath.Directory, "Plugins", "GameFeatures");
		var gameFeaturesModsRoot = DirectoryReference.Combine(ProjectParams.RawProjectPath.Directory, "Mods", "GameFeatures");
		return ProjectParams.DLCFile.IsUnderDirectory(gameFeaturesPluginsRoot) ||
			ProjectParams.DLCFile.IsUnderDirectory(gameFeaturesModsRoot);
	}

	private static string GetCanonicalModPathRelativeToRoot(ProjectParams ProjectParams)
	{
		// All DLC paths are remapped to projectName/Mods/DLCName during RemapCookedPluginsContentPaths, regardless of nesting
		// so the relative stage path is projectName/Mods/DLCName
		var projectName = ProjectParams.RawProjectPath.GetFileNameWithoutAnyExtensions();
		
		// Game features have to stay under Mods/GameFeatures to be picked up by the game as such
		var dlcName = ProjectParams.DLCFile.GetFileNameWithoutAnyExtensions();
		return IsGameFeatureDLC(ProjectParams) ? CombinePaths(projectName, "Mods", "GameFeatures", dlcName) : CombinePaths(projectName, "Mods", dlcName);
	}

	private static void RemapCookedPluginsContentPaths(ProjectParams ProjectParams, DeploymentContext SC) {
		//We need to make sure content paths will be relative to ../../../ProjectRoot/Mods/DLCFilename/Content,
		//because that's what will be used in runtime as a content path for /DLCFilename/ mount point.
		//But both project and engine plugins are actually cooked by different paths:
		//Project plugins expect to be mounted under ../../../ProjectRoot/Plugins/DLCFilename/Content,
		//and engine plugins expect to be mounted under ../../../Engine/Plugins/DLCFilename/Content
		//Since changing runtime content path is pretty complicated and impractical,
		//we remap cooked filenames to match runtime expectations. Since cooked assets only reference other assets
		//using mount point-relative paths, it doesn't need any changes inside of the cooked assets
		//deploymentContext.RemapDirectories.Add(Tuple.Create());

		SC.RemapDirectories.Add(Tuple.Create(
			SC.GetStagedFileLocation(ProjectParams.DLCFile).Directory,
			new StagedDirectoryReference(GetCanonicalModPathRelativeToRoot(ProjectParams))
		));
	}

	private void MarkGameFeature(ProjectParams ProjectParams, DeploymentContext SC)
	{
		if (!IsGameFeatureDLC(ProjectParams))
			return;

	}

	private DirectoryReference GetTempDirForStaging(ProjectParams ProjectParams, DeploymentContext SC)
	{
		return DirectoryReference.Combine(ProjectParams.DLCFile.Directory, "Intermediate", "Staging", SC.FinalCookPlatform);
	}
}

}

