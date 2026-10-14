using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using AutomationTool;
using Tools.DotNETCommon;

namespace Alpakit.Automation {

public class ITR2PackageFolders : BuildCommand
{
    public override void ExecuteBuild()
    {
        string project = Path.GetFullPath(ParseParamValue("project") ?? throw new AutomationException("Missing -project"));
        string selection = ParseParamValue("Selection") ?? throw new AutomationException("Missing -Selection");
        var json = JsonObject.Read(new FileReference(selection));
        string name = json.GetStringField("ModName");
        if (!Regex.IsMatch(name, "^[A-Za-z0-9][A-Za-z0-9_-]{0,63}$"))
            throw new AutomationException("Mod name must contain only letters, digits, underscores or hyphens (1-64 characters).");
        string projectDir = Path.GetDirectoryName(project);
        string projectName = Path.GetFileNameWithoutExtension(project);
        string content = Path.Combine(projectDir, "Content");
        string[] folders = json.GetStringArrayField("Folders").Distinct(StringComparer.OrdinalIgnoreCase).ToArray();
        string[] assets = json.TryGetStringArrayField("Assets", out var selectedAssets) ? selectedAssets : Array.Empty<string>();
        if (folders.Length == 0 && assets.Length == 0) throw new AutomationException("Select folders or assets.");
        bool patchSuffix = !json.TryGetBoolField("PatchSuffix", out var patch) || patch;
        bool autoInstall = json.TryGetBoolField("AutoInstall", out var install) && install;
        string installDirectory = json.TryGetStringField("InstallDirectory", out var directory) ? directory : "";
        if (autoInstall && (string.IsNullOrWhiteSpace(installDirectory) || !Path.IsPathRooted(installDirectory)))
            throw new AutomationException("Choose an absolute install directory.");
        string[] relative = folders.Select(folder =>
        {
            string normalized = folder.Replace('\\', '/').TrimEnd('/');
            if (normalized != "/Game" && !normalized.StartsWith("/Game/", StringComparison.Ordinal))
                throw new AutomationException("Only /Game folders are supported: {0}", folder);
            string suffix = normalized.Length == 5 ? "" : normalized.Substring(6);
            if (suffix.Split('/').Any(part => part == ".." || part == ".") || suffix.IndexOfAny(new[] {'"', ':', '\r', '\n'}) >= 0)
                throw new AutomationException("Invalid folder: {0}", folder);
            string full = Path.GetFullPath(Path.Combine(content, suffix));
            if (full != content && !full.StartsWith(content + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase))
                throw new AutomationException("Folder escapes project Content: {0}", folder);
            if (!Directory.Exists(full)) throw new AutomationException("Folder does not exist: {0}", full);
            return suffix;
        }).ToArray();
        var individualFiles = assets.Select(asset =>
        {
            if (!asset.StartsWith("/Game/", StringComparison.Ordinal)) throw new AutomationException("Only /Game assets are supported: {0}", asset);
            string suffix = asset.Substring(6);
            if (suffix.Split('/').Any(part => part == ".." || part == ".") || suffix.IndexOfAny(new[] {'"', ':', '\r', '\n'}) >= 0)
                throw new AutomationException("Invalid asset: {0}", asset);
            string full = Path.GetFullPath(Path.Combine(content, suffix));
            if (!full.StartsWith(content + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)) throw new AutomationException("Invalid asset: {0}", asset);
            if (File.Exists(full + ".uasset")) return full + ".uasset";
            if (File.Exists(full + ".umap")) return full + ".umap";
            throw new AutomationException("Asset does not exist: {0}", asset);
        }).ToArray();
        string run = Path.Combine(projectDir, "Saved", "AlpakitNonPluginMods", name, DateTime.UtcNow.ToString("yyyyMMdd-HHmmss") + "-" + Guid.NewGuid().ToString("N").Substring(0, 8));
        string cooked = Path.Combine(run, "Cooked");
        string output = Path.Combine(run, "Output");
        Directory.CreateDirectory(output);
        string engine = Path.Combine(CmdEnv.LocalRoot, "Engine");
        string editor = Path.Combine(engine, "Binaries", "Win64", "UE4Editor-Cmd.exe");
        string pak = Path.Combine(engine, "Binaries", "Win64", "UnrealPak.exe");
        string Q(string value) => "\"" + value + "\"";
        void Execute(string exe, string args)
        {
            var result = Run(exe, args);
            if (result.ExitCode != 0) throw new AutomationException("{0} failed with exit code {1}; retained work files: {2}", exe, result.ExitCode, run);
        }
        string cookArgs = Q(project) + " -run=Cook -TargetPlatform=WindowsNoEditor -unattended -stdout -UTF8Output -CookSinglePackageNoRefs -OutputDir=" + Q(cooked)
            + " -ini:Game:[/Script/UnrealEd.ProjectPackagingSettings]:bShareMaterialShaderCode=False";
        string[] sourcePackages = relative.SelectMany(folder => Directory.GetFiles(Path.Combine(content, folder), "*", SearchOption.AllDirectories))
            .Where(file => Path.GetExtension(file) is ".uasset" or ".umap").Concat(individualFiles).Distinct(StringComparer.OrdinalIgnoreCase)
            .Select(file => "/Game/" + Path.ChangeExtension(Compat.RelativePath(content, file), null).Replace('\\', '/')).ToArray();
        if (sourcePackages.Length == 0) throw new AutomationException("No assets selected.");
        string batch = cookArgs;
        foreach (string package in sourcePackages)
        {
            string argument = " -Map=" + Q(package);
            if (batch.Length + argument.Length > 24000)
            {
                Execute(editor, batch);
                batch = cookArgs + " -iterate";
            }
            batch += argument;
        }
        Execute(editor, batch);
        string cookedContent = Path.Combine(cooked, projectName, "Content");
        if (!Directory.Exists(cookedContent)) throw new AutomationException("Cooked Content not found: {0}", cookedContent);
        var selectedPackages = new System.Collections.Generic.HashSet<string>(sourcePackages, StringComparer.OrdinalIgnoreCase);
        string[] headers = Directory.GetFiles(cookedContent, "*", SearchOption.AllDirectories)
            .Where(file => Path.GetExtension(file) is ".uasset" or ".umap")
            .Where(file => selectedPackages.Contains("/Game/" + Path.ChangeExtension(Compat.RelativePath(cookedContent, file), null).Replace('\\', '/')))
            .OrderBy(file => file, StringComparer.OrdinalIgnoreCase).ToArray();
        if (headers.Length == 0) throw new AutomationException("No selected assets were cooked; work files: {0}", run);
        var response = new StringBuilder();
        foreach (string header in headers)
        {
            foreach (string file in new[] {header, Path.ChangeExtension(header, ".uexp"), Path.ChangeExtension(header, ".ubulk"), Path.ChangeExtension(header, ".uptnl")})
                if (File.Exists(file)) response.AppendLine(Q(file) + " " + Q("../../../" + projectName + "/Content/" + Compat.RelativePath(cookedContent, file).Replace('\\', '/')));
        }
        string responsePath = Path.Combine(run, "IoStoreFiles.txt");
        File.WriteAllText(responsePath, response.ToString(), new UTF8Encoding(false));
        string basename = patchSuffix && !name.EndsWith("_P", StringComparison.OrdinalIgnoreCase) ? name + "_P" : name;
        Execute(pak, Q(Path.Combine(output, basename + ".pak")) + " -Create=" + Q(responsePath));
        File.WriteAllLines(Path.Combine(run, "SelectedPackages.txt"), headers.Select(file => "/Game/" + Path.ChangeExtension(Compat.RelativePath(cookedContent, file), null).Replace('\\', '/')));
        if (autoInstall)
        {
            Directory.CreateDirectory(installDirectory);
            foreach (string extension in new[] {".pak"})
                File.Copy(Path.Combine(output, basename + extension), Path.Combine(installDirectory, basename + extension), true);
        }
        LogInformation("Packaged {0}: {1}", basename, output);
    }
}

}
