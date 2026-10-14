using System;
using System.Diagnostics;
using System.IO;
using AutomationTool;


namespace Alpakit.Automation {

public class LaunchGame
{
    public enum LaunchType
    {
        Steam,
        Custom
    }
    
    private static string GetGameLaunchUrl(LaunchType LaunchType)
    {
        switch (LaunchType)
        {
            case LaunchType.Steam:
                return "steam://rungameid/1012790";
            default:
                throw new AutomationException("Invalid Launch Type {0}", LaunchType);
        }
    }

    public static void Launch(LaunchType Type, string CustomLaunchPath, string CustomLaunchArgs, Action<string> logger)
    {
        if (Type == LaunchType.Custom)
        {
            if (CustomLaunchPath == null)
                throw new AutomationException("Custom Launch Type requested, but no program to launch was specified");

            ProcessStartInfo processInfo = new ProcessStartInfo()
            {
                FileName = CustomLaunchPath,
                Arguments = CustomLaunchArgs,
                UseShellExecute = true
            };

            logger($"Custom Launch starting process: `{processInfo.FileName}` with arguments `{processInfo.Arguments}`");
            Process.Start(processInfo);
            return;
        }

        Process.Start(new ProcessStartInfo(GetGameLaunchUrl(Type)) { UseShellExecute = true });
    }
}

}


