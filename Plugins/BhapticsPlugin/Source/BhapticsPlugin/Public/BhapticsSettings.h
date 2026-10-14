//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreMinimal.h"
#include "BhapticsStructs.h"

#include "BhapticsSettings.generated.h"

UCLASS()
class BHAPTICSPLUGIN_API UBhapticsSettings : public UObject
{
	GENERATED_BODY()

public :
	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	FString AppName = "";

	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	FString AppId = "";

	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	FString ApiKey = "";

	UPROPERTY(EditAnywhere, Category = "Bhaptics Setting")
	bool AutoLaunchPlayer = true;

	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	int LastDeployVersion = 0;

	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	TArray<FBhapticsMappingMetaData> EventList;

	UPROPERTY(VisibleAnywhere, Category = "Bhaptics Setting")
	FString DefaultConfig = "";

};
