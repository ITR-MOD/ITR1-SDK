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
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
class MessageSender
{
public:
	static FString GetInitMessage(const FString& AppId, const FString& ApiKey);
	static FString GetPingToServerMessage();
	
	static FString GetPlayEventMessage(
		const FString& EventName,
		const int RequestId,
		const float Intensity,
		const float Duration,
		const float OffsetAngleX,
		const float OffsetY
		);
	static FString GetPlayLoopEventMessage(
		const FString& EventName,
		const int RequestId,
		const float Intensity,
		const float Duration,
		const float OffsetAngleX,
		const float OffsetY,
		const int Interval,
		const int MaxCount
		);
	static FString GetPlayDotEventMessage(
		const int RequestId,
		const int Pos,
		const int DurationMillis,
		const TArray<int>& MotorValues
		);
	static FString GetPlayPathEventMessage(
		const int RequestId,
		const int Pos,
		const int DurationMillis,
		const TArray<float>& X,
		const TArray<float>& Y,
		const TArray<int>& MotorValues
		);
	static FString GetPlayWaveformEventMessage(
		const int RequestId,
		const int Pos,
		const TArray<int>& PlayTimeValues,
		const TArray<int>& ShapeValues,
		const TArray<int>& MotorValues
		);
	
	static FString GetStopAllMessage();
	static FString GetStopByEventNameMessage(const FString& EventName);
	static FString GetStopByRequestIdMessage(const int RequestId);
	
	static void TryParseServerMessage(const FString& JsonString);
	static int32 TryParseUDPMessage(const FString& JsonString);


	static FString StringFromBinaryArray(TArray<uint8>& BinaryArray);
private:

	static FString GenerateMessage(const FString& MessageType, const TSharedPtr<FJsonObject>& Message);
	static FString GenerateMessage(const FString& MessageType, const FString& Message);
};