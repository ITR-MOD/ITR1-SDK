#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "AlpakitSettings.generated.h"

UENUM()
enum class EAlpakitStartGameType : uint8 {
    STEAM UMETA(DisplayName = "Steam"),
    CUSTOM UMETA(DisplayName = "Custom")
};

USTRUCT()
struct ALPAKIT_API FAlpakitTargetSettings
{
    GENERATED_BODY()
public:
	
    UPROPERTY(EditAnywhere, config, Category = Config, meta = (ToolTip = "Controls if this target will be built when pressing the 'Alpakit Dev' button (or Alpakit! for one mod) and what action will be taken for this target after all tasks complete."))
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, config, Category = Config, meta = (InlineEditConditionToggle))
    bool bCopyModsToGame = false;
	
    UPROPERTY(EditAnywhere, config, Category = Config, DisplayName = "Copy to Game Path", meta = (EditCondition = bCopyModsToGame, ToolTip = "If enabled, mods will be copied to this installation location after a successful pack.\n\nExample value:\nC:\\Program Files (x86)\\Steam\\steamapps\\common\\IntoTheRadius2"))
    FDirectoryPath ITR2GamePath;

    UPROPERTY(EditAnywhere, config, Category = Config, meta = (InlineEditConditionToggle))
    bool bLaunchGame = false;

    UPROPERTY(EditAnywhere, config, Category = Config, meta = ( EditCondition = bLaunchGame, ToolTip = "If enabled, this approach will be used to launch this target after a successful pack"))
    EAlpakitStartGameType LaunchGameType;

    UPROPERTY(EditAnywhere, config, Category = Config, meta = ( EditCondition = "LaunchGameType == EAlpakitStartGameType::CUSTOM", EditConditionHides, ToolTip = "When 'Custom' is selected, the Unreal Automation tool will execute this after a successful pack.\nCheck the 'Testing' page on the modding docs for example scripts to call with this feature.\n\nExample value:\npowershell"))
    FString CustomLaunchPath;

    UPROPERTY(EditAnywhere, config, Category = Config, meta = ( EditCondition = "LaunchGameType == EAlpakitStartGameType::CUSTOM", EditConditionHides, ToolTip = "When 'Custom' is selected, these arguments will be used when executing the above after a successful pack.\nCheck the 'Testing' page on the modding docs for example scripts to call with this feature.\n\nExample value:\nC:\\Git\\SatisfactoryModLoader\\Automation\\SFLaunch.ps1 -branch EA -loadLatestSave"))
    FString CustomLaunchArgs;
};

ALPAKIT_API const TCHAR* LexToString(EAlpakitStartGameType StartGameType);

UCLASS(config=Alpakit)
class ALPAKIT_API UAlpakitSettings : public UObject {
    GENERATED_BODY()
public:
    /** Retrieves global instance of alpakit settings */
    static UAlpakitSettings* Get();

    /** Saves alpakit settings to configuration file */
    void SaveSettings();

    UPROPERTY(EditAnywhere, config, Category="Advanced Settings", meta=(ToolTip="Release version containing the base game's AssetRegistry.bin under Releases/<version>/WindowsNoEditor. Required for DLC cooking."))
    FString BasedOnReleaseVersion = TEXT("ITRExternalBase");

    UPROPERTY(EditAnywhere, config, Category="Plugin Installation", DisplayName="Auto Install Plugins")
    bool bAutoInstallPlugins = false;

    UPROPERTY(EditAnywhere, config, Category="Plugin Installation", meta=(EditCondition="bAutoInstallPlugins", ToolTip="Each packaged plugin is installed into a subfolder named after the plugin."))
    FDirectoryPath PluginInstallDirectory;

    UPROPERTY(EditAnywhere, config, Category = "Dev Packaging Settings", DisplayName = "Windows", meta = (ToolTip = "Development-time packaging settings for the Windows target"))
    FAlpakitTargetSettings WindowsGameTargetSettings;

    UPROPERTY(EditAnywhere, config, Category = "Dev Packaging Settings", DisplayName="Windows Server", meta = (ToolTip = "Development-time packaging settings for the Windows Server target"))
    FAlpakitTargetSettings WindowsServerTargetSettings;

    UPROPERTY(EditAnywhere, config, Category = "Dev Packaging Settings", DisplayName="Linux Server", meta = (ToolTip = "Development-time packaging settings for the Linux Server target"))
    FAlpakitTargetSettings LinuxServerTargetSettings;

	// Name of the build configuration in which the mod should be built.
	// This is exclusively for internal CSS use for now, there are no publicly available builds in configurations other than Shipping.
	// Default value is Shipping when left blank
	UPROPERTY(EditAnywhere, config, Category = "Advanced Settings", meta = ( GetOptions = GetAllowedBuildConfigurations ))
	FString OverrideBuildConfiguration;

	// Name of the targets for which the mod should be built. When left empty, default values are used (FactoryServer, FactoryGameSteam and FactoryGameEGS)
	UPROPERTY(EditAnywhere, config, Category = "Advanced Settings")
	TArray<FString> OverrideBuildTargets;

    UPROPERTY(BlueprintReadOnly, config, Category = Config)
    TMap<FString, bool> ModSelection;

    UPROPERTY(BlueprintReadOnly, config, Category = Config)
    bool OrderByFriendlyName;

	UFUNCTION()
	TArray<FString> GetAllowedBuildConfigurations() const;

	EBuildConfiguration GetBuildConfiguration() const;

    const TMap<FString, FAlpakitTargetSettings> GetPlatformTargetSettings() const;
};
