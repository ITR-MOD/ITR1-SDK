//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreMinimal.h"
#include "BhapticsStructs.h"
#include "BhapticsSettings.h"

#include "BhapticsPluginEditorSettings.generated.h"


USTRUCT()
struct FBhapticsEventListMessage {

	GENERATED_BODY();

	UPROPERTY() bool Status;
	UPROPERTY() TArray<FBhapticsMappingMetaData> Message;
};


USTRUCT()
struct FBhapticsDefaultConfig {

	GENERATED_BODY();

	UPROPERTY() FString Name;
	UPROPERTY() int Version;
	UPROPERTY() bool AutoLaunchPlayer;
};

struct BhapticsDefaultConfig {
	FString Name;
	int Version;
	bool bAutoLaunchPlayer;
	FString ConfigString;
};



USTRUCT()
struct FBhapticsDefaultConfigMessage {

	GENERATED_BODY();

	UPROPERTY() bool Status;
	UPROPERTY() FBhapticsDefaultConfig Message;
};



UCLASS(config = Editor, defaultconfig)
class BHAPTICSPLUGINEDITOR_API UBhapticsPluginEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "Bhaptics Setting", meta = (EditCondition = "Status!=0", EditConditionHides))
		FString AppId;
	UPROPERTY(config, EditAnywhere, Category = "Bhaptics Setting", meta = (EditCondition = "Status!=0", EditConditionHides))
		FString ApiKey;
	UPROPERTY(config, BlueprintReadOnly, Category = "Bhaptics Setting")
		int Status = -1;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Bhaptics Setting", meta = (AllowedClasses = "/Script/BhapticsPlugin.BhapticsSettings", ConfigRestartRequired = true, EditCondition = "Status==0", EditConditionHides))
		FSoftObjectPath  Setting;
};
