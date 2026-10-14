//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreMinimal.h"

#include "BhapticsGameConfig.generated.h"

UCLASS(config = Game, defaultconfig)
class BHAPTICSPLUGIN_API UBhapticsGameConfig : public UObject
{
	GENERATED_BODY()

public :
	UPROPERTY(config, EditAnywhere, Category = "Bhaptics Setting", meta = (AllowedClasses = "/Script/BhapticsPlugin.BhapticsSettings"))
		FSoftObjectPath  Configuration;
};
