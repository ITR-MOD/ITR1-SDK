//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreTypes.h"
#include "CoreMinimal.h"
#include "BhapticsStructs.generated.h"

UENUM(BlueprintType)
enum class EBhapticsDevicePosition : uint8
{
	Vest, ForearmL, ForearmR, Head, HandL, HandR, FootL, FootR, GloveL, GloveR
};

UENUM(BlueprintType)
enum class EBhapticsGloveShapeValue : uint8 {
	Constant = 0, Decreasing = 1, Increasing = 2
};

UENUM(BlueprintType)
enum class EBhapticsGlovePlayTime : uint8 {
	None = 0, FiveMS = 1, TenMS = 2, TwentyMS = 4, ThirtyMS = 6, FortyMS = 8
};


USTRUCT(BlueprintType)
struct FBhapticsRotationOption
{
	GENERATED_BODY()

	FBhapticsRotationOption()
	{
		OffsetAngleX = 0;
		OffsetY = 0;
	}

	//Rotate the feedback file horizontally clockwise along the vest by the given angle in degrees.
	//A value of 180 will flip the feedback to the other side of the device.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vars)
		float OffsetAngleX = 0;

	//Vertical offset of the transformed feedback file, with negative values moving the feedback upwards.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vars)
		float OffsetY = 0;

	FBhapticsRotationOption(float AngleX, float Y)
	{
		OffsetAngleX = AngleX;
		OffsetY = FMath::Clamp(Y, -1.0f, 1.0f);
	}
};

USTRUCT(BlueprintType)
struct FBhapticsMappingMetaData {

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	int32 DurationMillis = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	FString Key = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	FString Description = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	bool IsAudio = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	int64 UpdateTime = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	TArray<FString> Positions;
};




USTRUCT(BlueprintType)
struct FBhapticsDevice {

	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	FString DeviceName = "";

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	FString Address = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	int32 Position = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	bool Connected = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	bool Paired = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	int32 Battery = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bHaptics")
	bool AudioJackIn = false;
};
