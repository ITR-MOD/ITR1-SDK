/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */
#include "BhapticsTactHubGameSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "TactHub/MessageSender.h"
#include "TactHub/TcpConnector.h"
#include "TactHub/UDPListener.h"

#if PLATFORM_WINDOWS
	#if WITH_EDITOR
	#define USE_TACTHUB 1
	#else
	#define USE_TACTHUB 0
	#endif

#elif  PLATFORM_ANDROID  
#define USE_TACTHUB 0
#else
#define USE_TACTHUB 1
#endif


TcpConnector Connector;

void UBhapticsTactHubGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
#if USE_TACTHUB == 0
	return;
#else
	Super::Initialize(Collection);
	
	SetTimer();

	UDPListener::SetupUDP();
#endif
}

void UBhapticsTactHubGameSubsystem::Deinitialize()
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	SendPingToServer();

	Super::Deinitialize();
	Connector.CloseConnection();

	UDPListener::DestroyUDP();
#endif
}

void UBhapticsTactHubGameSubsystem::InitializeSDK(const FString& AppId, const FString& ApiKey)
{
#if USE_TACTHUB == 0
	return;
#else
	UE_LOG(LogTemp, Log, TEXT("InitializeSDK: %s"), *AppId);
	Connector = TcpConnector(AppId, ApiKey);
#endif
}


void UBhapticsTactHubGameSubsystem::OnCheck()
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (Connector.IsConnected)
	{
		Connector.TryReadData();
	} else
	{
		const auto ConnectInfo = UDPListener::TryReceiveUDPData();
		if (ConnectInfo.Valid)
		{
			Connector.ConnectToServer(ConnectInfo.SenderIp, ConnectInfo.Port);
		}
	}
#endif
}

void UBhapticsTactHubGameSubsystem::SetTimer()
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (GetWorld())
	{
		float Interval = 1.0f;
		// Set the timer to call the static function
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, []()
		{
			OnCheck();
			SendPingToServer();
		}, Interval, true);
	}
#endif
}

void UBhapticsTactHubGameSubsystem::SendPingToServer()
{
	if (Connector.IsConnected)
	{
		const auto Message = MessageSender::GetPingToServerMessage();
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}
	}
}



int UBhapticsTactHubGameSubsystem::PlayHaptic(const FString& EventName)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	return PlayHapticWithOptions(EventName, 1, 1, 0, 0);
#endif
}

int UBhapticsTactHubGameSubsystem::PlayHapticWithOptions(
	const FString& EventName, const float IntensityRatio, const float DurationRatio,
	const float OffsetAngleX, const float OffsetY)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	if (Connector.IsConnected)
	{
		const int RequestId = GetRequestId();
    	
		const auto Message = MessageSender::GetPlayEventMessage(
			EventName,
			RequestId,
			IntensityRatio,
			DurationRatio,
			OffsetAngleX,
			OffsetY
			);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}

		return RequestId;
	}

	return -1;
#endif
}

int UBhapticsTactHubGameSubsystem::PlayLoopHapticWithOptions(
	const FString& EventName, const float IntensityRatio, const float DurationRatio,
	const float OffsetAngleX, const float OffsetY, const int Interval, const int MaxCount)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	if (Connector.IsConnected)
	{
		const int RequestId = GetRequestId();
    	
		const auto Message = MessageSender::GetPlayLoopEventMessage(
			EventName,
			RequestId,
			IntensityRatio,
			DurationRatio,
			OffsetAngleX,
			OffsetY,
			Interval,
			MaxCount
			);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}

		return RequestId;
	}

	return -1;
#endif
}


int UBhapticsTactHubGameSubsystem::PlayMotors(
	const int Pos, const int DurationMillis,
	const TArray<int>& Motors)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	if (Connector.IsConnected)
	{
		const int RequestId = GetRequestId();
    	
		const auto Message = MessageSender::GetPlayDotEventMessage(
			RequestId,
			Pos,
			DurationMillis,
			Motors);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}

		return RequestId;
	}

	return -1;
#endif
}


int UBhapticsTactHubGameSubsystem::PlayPath(
	const int Pos, const int DurationMillis,
	const TArray<float>& X,
	const TArray<float>& Y,
	const TArray<int>& Motors)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	if (Connector.IsConnected)
	{
		const int RequestId = GetRequestId();
    	
		const auto Message = MessageSender::GetPlayPathEventMessage(
			RequestId,
			Pos,
			DurationMillis,
			X, Y,
			Motors);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}

		return RequestId;
	}

	return -1;
#endif
}
int UBhapticsTactHubGameSubsystem::PlayWaveform(
	const int Pos,
	const TArray<EBhapticsGlovePlayTime>& PlayTimeValues,
	const TArray<EBhapticsGloveShapeValue>& ShapeValues,
	const TArray<int>& Motors)
{
#if USE_TACTHUB == 0
	return -1;
#else
	
	if (Connector.IsConnected)
	{
		const int RequestId = GetRequestId();

		const auto Message = MessageSender::GetPlayWaveformEventMessage(
			RequestId,
			Pos,
			ConvertEnumArrayToIntArray(PlayTimeValues), ConvertEnumArrayToIntArray(ShapeValues),
			Motors);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}

		return RequestId;
	}

	return -1;
#endif
}


void UBhapticsTactHubGameSubsystem::StopByRequestId(const int RequestId)
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (Connector.IsConnected)
	{
		const auto Message = MessageSender::GetStopByRequestIdMessage(RequestId);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}
	}
#endif
}

void UBhapticsTactHubGameSubsystem::StopByEventName(const FString& EventName)
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (Connector.IsConnected)
	{
		const auto Message = MessageSender::GetStopByEventNameMessage(EventName);
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}
	}
#endif
}

void UBhapticsTactHubGameSubsystem::StopAll()
{
#if USE_TACTHUB == 0
	return;
#else
	
	if (Connector.IsConnected)
	{
		const auto Message = MessageSender::GetStopAllMessage();
		const bool Res = Connector.SendData(Message);

		if (!Res)
		{
			Connector.DisposeConnection();
		}
	}
#endif
}

int UBhapticsTactHubGameSubsystem::GetRequestId()
{
	const int32 RequestId = FMath::RandRange(1, 1000000);
	return RequestId;
}

TArray<int> UBhapticsTactHubGameSubsystem::ConvertEnumArrayToIntArray(const TArray<EBhapticsGlovePlayTime>& EnumArray)
{
	TArray<int> IntArray;
	for (EBhapticsGlovePlayTime EnumValue : EnumArray)
	{
		IntArray.Add(static_cast<int>(EnumValue));
	}
	return IntArray;
}

TArray<int> UBhapticsTactHubGameSubsystem::ConvertEnumArrayToIntArray(const TArray<EBhapticsGloveShapeValue>& EnumArray)
{
	TArray<int> IntArray;
	for (EBhapticsGloveShapeValue EnumValue : EnumArray)
	{
		IntArray.Add(static_cast<int>(EnumValue));
	}
	return IntArray;
}
