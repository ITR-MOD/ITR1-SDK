/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "BhapticsStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BhapticsTactHubGameSubsystem.generated.h"

UCLASS()
class UBhapticsTactHubGameSubsystem  : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	static void InitializeSDK(const FString& AppId, const FString& ApiKey);
	static int PlayHaptic(const FString& EventName);
	static int PlayHapticWithOptions(const FString& EventName,
		float IntensityRatio = 1, float DurationRatio = 1,
		float OffsetAngleX = 0, float OffsetY = 0
		);

	static int PlayLoopHapticWithOptions(
		const FString& EventName, const float IntensityRatio, const float DurationRatio,
		const float OffsetAngleX, const float OffsetY, const int Interval, const int MaxCount);

	static int PlayMotors(
		const int Pos, const int DurationMillis,
		const TArray<int>& Motors);
	
	static int PlayPath(
		const int Pos, const int DurationMillis,
		const TArray<float>& X,
		const TArray<float>& Y,
		const TArray<int>& Motors);

	static int PlayWaveform(
		const int Pos,
		const TArray<EBhapticsGlovePlayTime>& PlayTimeValues, const TArray<EBhapticsGloveShapeValue>& ShapeValues,
		const TArray<int>& Motors);
	
	static void StopByRequestId(int RequestId);
	static void StopByEventName(const FString& EventName);
	static void StopAll();


private:
	FTimerHandle TimerHandle;
	void SetTimer();
	static void OnCheck();
	static void SendPingToServer();

	static int GetRequestId();

	static TArray<int> ConvertEnumArrayToIntArray(const TArray<EBhapticsGlovePlayTime>& EnumArray);
	static TArray<int> ConvertEnumArrayToIntArray(const TArray<EBhapticsGloveShapeValue>& EnumArray);
};
