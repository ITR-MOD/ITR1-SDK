#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "FolderModSettings.generated.h"

USTRUCT()
struct FNonPluginModConfiguration
{
    GENERATED_BODY()
    UPROPERTY() FString ModName;
    UPROPERTY() TArray<FDirectoryPath> Folders;
    UPROPERTY() TArray<TSoftObjectPtr<UObject>> Assets;
    UPROPERTY() bool bPatchSuffix = true;
    UPROPERTY() bool bAutoInstall = false;
    UPROPERTY() FDirectoryPath InstallDirectory;
};

UCLASS(Config=EditorPerProjectUserSettings)
class UFolderModSettings : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Config, Category="Non-Plugin Mod")
    FString ModName = TEXT("FolderMod");

    UPROPERTY(EditAnywhere, Config, Category="Selection", meta=(ContentDir, LongPackageName))
    TArray<FDirectoryPath> Folders;

    UPROPERTY(EditAnywhere, Config, Category="Selection")
    TArray<TSoftObjectPtr<UObject>> Assets;

    UPROPERTY(EditAnywhere, Config, Category="Packaging", meta=(DisplayName="Append _P"))
    bool bPatchSuffix = true;

    UPROPERTY(EditAnywhere, Config, Category="Installation")
    bool bAutoInstall = false;

    UPROPERTY(EditAnywhere, Config, Category="Installation", meta=(EditCondition="bAutoInstall"))
    FDirectoryPath InstallDirectory;

    UPROPERTY(Config) TArray<FNonPluginModConfiguration> Configurations;
    UPROPERTY(Config) int32 SelectedConfiguration = 0;

    void SaveSelection();
    void LoadSelection(int32 Index);
};

void ShowFolderModWindow();
