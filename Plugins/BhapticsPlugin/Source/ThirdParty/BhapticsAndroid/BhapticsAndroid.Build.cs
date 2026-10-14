//Copyright bHaptics Inc. All Rights Reserved 2022~

using System;
using System.IO;
using UnrealBuildTool;

public class BhapticsAndroid : ModuleRules
{
    public BhapticsAndroid(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;


        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicIncludePathModuleNames.AddRange(
                new string[] {
                    "Launch",
                });

            
            string pluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            string aplPath = Path.Combine(pluginPath, "BHapticsAndroid_APL.xml");

            AdditionalPropertiesForReceipt.Add("AndroidPlugin", aplPath);
            Console.WriteLine("BhapticsAndroid Final Android APL path: " + aplPath);
        }
    }
}
