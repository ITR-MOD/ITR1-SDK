/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */
#include "MessageSender.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/LogMacros.h"

FPluginDescriptor GetPluginVersion()
{
	const FString PluginName = "BhapticsPlugin";
	// Get the plugin manager instance
	IPluginManager& PluginManager = IPluginManager::Get();

	// Find the plugin by name
	TSharedPtr<IPlugin> Plugin = PluginManager.FindPlugin(PluginName);
	if (Plugin.IsValid())
	{
		// Get the plugin version
		FPluginDescriptor Descriptor = Plugin->GetDescriptor();
		return Descriptor;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Plugin '%s' not found."), *PluginName);

		FPluginDescriptor Descriptor;
		return Descriptor;
	}
}

void MakeHttpRequest(const FString& Token, const FString& TokenKey)
{
	const FString Url = FString::Printf(TEXT("https://sdk-apis.bhaptics.com/api/v1/tacthub-api/verify?token=%s&token-key=%s"), *Token, *TokenKey);
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			// Handle the response
			const FString ResponseString = Response->GetContentAsString();
			UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);
		}
		else
		{
			// Handle the error
			UE_LOG(LogTemp, Error, TEXT("HTTP Request Failed"));
		}
	});

	Request->ProcessRequest();
}

FString MessageSender::GetInitMessage(const FString& AppId, const FString& ApiKey)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("applicationId"), AppId);
	JsonObject->SetStringField(TEXT("sdkApiKey"), ApiKey);
	FPluginDescriptor Descriptor = GetPluginVersion();
	JsonObject->SetNumberField(TEXT("sdkVersion"), Descriptor.Version);

	FString VersionMessage = FString::Printf(TEXT("Unreal Engine-bHaptics-%s"), *Descriptor.VersionName);
	
	JsonObject->SetStringField(TEXT("sdkVersionName"),  VersionMessage);

	const FString MessageType = TEXT("SdkRequestAuthInit");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPingToServerMessage()
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	const FString MessageType = TEXT("SdkPingToServer");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPlayEventMessage(
	const FString& EventName,
	const int RequestId,
	const float Intensity,
	const float Duration,
	const float OffsetAngleX,
	const float OffsetY
	)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("eventName"), EventName);
	JsonObject->SetNumberField(TEXT("requestId"), RequestId);
	JsonObject->SetNumberField(TEXT("intensity"), Intensity);
	JsonObject->SetNumberField(TEXT("duration"), Duration);
	JsonObject->SetNumberField(TEXT("offsetAngleX"), OffsetAngleX);
	JsonObject->SetNumberField(TEXT("offsetY"), OffsetY);

	const FString MessageType = TEXT("SdkPlay");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPlayLoopEventMessage(const FString& EventName, const int RequestId, const float Intensity,
	const float Duration, const float OffsetAngleX, const float OffsetY, const int Interval, const int MaxCount)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("eventName"), EventName);
	JsonObject->SetNumberField(TEXT("requestId"), RequestId);
	JsonObject->SetNumberField(TEXT("intensity"), Intensity);
	JsonObject->SetNumberField(TEXT("duration"), Duration);
	JsonObject->SetNumberField(TEXT("offsetAngleX"), OffsetAngleX);
	JsonObject->SetNumberField(TEXT("offsetY"), OffsetY);
	JsonObject->SetNumberField(TEXT("interval"), Interval);
	JsonObject->SetNumberField(TEXT("maxCount"), MaxCount);

	const FString MessageType = TEXT("SdkPlayLoop");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPlayDotEventMessage(const int RequestId, const int Pos, const int DurationMillis,
                                              const TArray<int>& MotorValues)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("requestId"), RequestId);
	JsonObject->SetNumberField(TEXT("pos"), Pos);
	JsonObject->SetNumberField(TEXT("durationMillis"), DurationMillis);

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const int Value : MotorValues)
	{
		// Wrap each integer in a TSharedPtr<FJsonValue>.
		JsonArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}

	// Set the array field with the converted array.
	JsonObject->SetArrayField(TEXT("motors"), JsonArray);

	const FString MessageType = TEXT("SdkPlayDotMode");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPlayPathEventMessage(const int RequestId, const int Pos, const int DurationMillis,
                                               const TArray<float>& X, const TArray<float>& Y, const TArray<int>& MotorValues)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("requestId"), RequestId);
	JsonObject->SetNumberField(TEXT("pos"), Pos);
	JsonObject->SetNumberField(TEXT("durationMillis"), DurationMillis);

	TArray<TSharedPtr<FJsonValue>> MotorValueArray;
	for (const int Value : MotorValues)
	{
		MotorValueArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("intensity"), MotorValueArray);

	
	TArray<TSharedPtr<FJsonValue>> XArray;
	for (const int Value : X)
	{
		XArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("x"), XArray);

	
	TArray<TSharedPtr<FJsonValue>> YArray;
	for (const int Value : Y)
	{
		YArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("y"), YArray);

	const FString MessageType = TEXT("SdkPlayPathMode");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetPlayWaveformEventMessage(const int RequestId, const int Pos, const TArray<int>& PlayTimeValues,
                                                   const TArray<int>& ShapeValues, const TArray<int>& MotorValues)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("requestId"), RequestId);
	JsonObject->SetNumberField(TEXT("pos"), Pos);

	TArray<TSharedPtr<FJsonValue>> MotorValueArray;
	for (const int Value : MotorValues)
	{
		MotorValueArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("motorValues"), MotorValueArray);
	
	TArray<TSharedPtr<FJsonValue>> ShapeArray;
	for (const int Value : ShapeValues)
	{
		ShapeArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("shapeValues"), ShapeArray);
	
	TArray<TSharedPtr<FJsonValue>> PlayTimeArray;
	for (const int Value : PlayTimeValues)
	{
		PlayTimeArray.Add(MakeShareable(new FJsonValueNumber(Value)));
	}
	JsonObject->SetArrayField(TEXT("playTimeValues"), PlayTimeArray);

	const FString MessageType = TEXT("SdkPlayWaveformMode");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetStopAllMessage()
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	const FString MessageType = TEXT("SdkStopAll");

	return GenerateMessage(MessageType, JsonObject);
}

FString MessageSender::GetStopByEventNameMessage(const FString& EventName)
{
	const FString MessageType = TEXT("SdkStopByEventId");

	return GenerateMessage(MessageType, EventName);
}

FString MessageSender::GetStopByRequestIdMessage(const int RequestId)
{
	const FString RequestIdFString = FString::FromInt(RequestId);
	const FString MessageType = TEXT("SdkStopByRequestId");

	return GenerateMessage(MessageType, RequestIdFString);
}

void MessageSender::TryParseServerMessage(const FString& JsonString)
{
	UE_LOG(LogTemp, Log, TEXT("TryParseServerMessage: %s"), *JsonString);
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const FString MessageType = JsonObject->GetStringField(TEXT("type"));

		UE_LOG(LogTemp, Warning, TEXT("Message Type: %s"), *MessageType);
		if (MessageType.Equals(TEXT("ServerTokenMessage"), ESearchCase::IgnoreCase))
		{
			const FString MessageString = JsonObject->GetStringField(TEXT("message"));

			TSharedPtr<FJsonObject> MessageObject;
			const TSharedRef<TJsonReader<>> MessageReader = TJsonReaderFactory<>::Create(MessageString);
			if (FJsonSerializer::Deserialize(MessageReader, MessageObject) && MessageObject.IsValid())
			{
				const FString Token = MessageObject->GetStringField(TEXT("token"));
				const FString TokenKey = MessageObject->GetStringField(TEXT("tokenKey"));

				UE_LOG(LogTemp, Warning, TEXT("Message Token: %s, TokenKey: %s"), *Token, *TokenKey);

				MakeHttpRequest(Token, TokenKey);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unhanded Type: %s"), *MessageType);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TryParse Type failed: %s"), JsonObject.IsValid() ? TEXT("True") : TEXT("False"));
	}
}

int32 MessageSender::TryParseUDPMessage(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const int32 Port = JsonObject->GetIntegerField(TEXT("port"));
		FString UserId = JsonObject->GetStringField(TEXT("userId"));

		return Port;
		// Now you can use 'Port' and 'UserId' as needed
	}
	// Handle the error if JSON parsing fails
	return -1;
}

FString MessageSender::StringFromBinaryArray(TArray<uint8>& BinaryArray)
{
	// There is an issue where dummy 0xdd is appended in PS, so this logic is added to remove it
	// Need JSON format ending with 0x7d
	while (BinaryArray.Num() > 0 && BinaryArray.Last() == 0xDD) {
		BinaryArray.Pop();
	}

	
	BinaryArray.Add(0);
	return FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

FString MessageSender::GenerateMessage(const FString& MessageType, const TSharedPtr<FJsonObject>& Message)
{
	FString MessageString;

	if (Message.IsValid())
	{
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&MessageString, 0); // 0 for no indentation
		FJsonSerializer::Serialize(Message.ToSharedRef(), Writer);
		Writer->Close(); // Ensure to close the writer

		MessageString.ReplaceInline(TEXT("\n"), TEXT(""));
		MessageString.ReplaceInline(TEXT("\r"), TEXT(""));
		MessageString.ReplaceInline(TEXT("\t"), TEXT(""));
	}
	return GenerateMessage(MessageType, MessageString);
}

FString MessageSender::GenerateMessage(const FString& MessageType, const FString& MessageString)
{
	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField(TEXT("message"), MessageString);
	JsonObject->SetStringField(TEXT("type"), MessageType);

	FString OutputString;

	const TSharedRef<TJsonWriter<>> OutputWriter = TJsonWriterFactory<>::Create(&OutputString, 0);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), OutputWriter);
	OutputWriter->Close();

	OutputString.ReplaceInline(TEXT("\n"), TEXT(""));
	OutputString.ReplaceInline(TEXT("\r"), TEXT(""));
	
	return OutputString + TEXT("\n");
}
