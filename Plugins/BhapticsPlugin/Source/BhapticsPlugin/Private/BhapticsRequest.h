//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "BhapticsStructs.h"

class BhapticsRequest
{
private:
public:

    static void Initialize(FString app, FString key, FString defaultConfig, bool bAutoLaunchPlayer);
    static void InitializePermissionOption(FString app, FString key, FString defaultConfig, bool requestPermission, bool bAutoLaunchPlayer);

    static void Destroy();

    static bool IsBhapticsAvailable(bool bAutoLaunchPlayer);


    static bool StopByEventId(FString eventId);
    static bool StopByRequestId(int requestId);
    static bool Stop();

    static int PlayHaptic(FString eventId);
    static int Play(FString eventId, float intensity, float duration, float angleX, float offsetY);
    static int PlayDot(int position, float dutation, TArray<int> motorValues);
    static int PlayPath(int Position, float DurationSeconds, const TArray<float>& X, const TArray<float>& Y, const TArray<int>& Intensities);
    static int PlayWaveform(int position, TArray<int> motorIntensities, TArray<EBhapticsGlovePlayTime> playTimeValues, TArray<EBhapticsGloveShapeValue> shapeValues);
    static int PlayLoop(FString eventId, float intensity, float duration, float angleX, float offsetY, int interval, int maxCount);

    static bool IsPlaying();
    static bool IsPlayingByRequestId(int requestId);
    static bool IsPlayingByEventId(FString eventId);

    static TArray<FBhapticsDevice> GetDevices();
    static void Ping(FBhapticsDevice device);
    static void PingAll();
    static void SwapPosition(FBhapticsDevice device);

    static FBhapticsRotationOption ProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight);
    static FBhapticsRotationOption ProjectToVestLocation(FVector ContactLocation, FVector PlayerLocation, FRotator PlayerRotation);
    static FBhapticsRotationOption CustomProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight, FVector UpVector, FVector ForwardVector);
};
