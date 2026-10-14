//Copyright bHaptics Inc. All Rights Reserved 2022~


#include "BhapticsSDK2.h" 
#if PLATFORM_WINDOWS
#include "BhapticsCPP/BhapticsCPP.h"
#endif 
#include "BhapticsRequest.h"
#include "BhapticsSettings.h"
#include "BhapticsGameConfig.h"

#include "BhapticsTactHubGameSubsystem.h"

bool Initialized = false;
bool IsConfigurationFailed = false;
bool autoRequestBluetoothPermission = true;

bool UBhapticsSDK2::AutoLaunchPlayer = true;

void UBhapticsSDK2::Initialize()
{
	if (Initialized) {

#if WITH_EDITOR
		UBhapticsSDK2::Destroy();
#else
		// TODO checck if it's working as expected
		return;
#endif
	}

	IsConfigurationFailed = false;

	UBhapticsSettings* Setting = TryLoadBhapticsConfig();
	if (Setting == NULL) 
	{
		IsConfigurationFailed = true;
		return;
	}

	AutoLaunchPlayer = Setting->AutoLaunchPlayer;
	BhapticsRequest::InitializePermissionOption(Setting->AppId, Setting->ApiKey, Setting->DefaultConfig, false, AutoLaunchPlayer);
	IsConfigurationFailed = false;

	UBhapticsTactHubGameSubsystem::InitializeSDK(Setting->AppId, Setting->ApiKey);

	Initialized = true;

}

bool UBhapticsSDK2::TryInitializeAndCheck() {
	if (!Initialized && !IsConfigurationFailed) {
		UBhapticsSDK2::Initialize();
	}
	return BhapticsRequest::IsBhapticsAvailable(AutoLaunchPlayer);
}

UBhapticsSettings* UBhapticsSDK2::TryLoadBhapticsConfig() {
	UBhapticsGameConfig* gameConfig = GetMutableDefault<UBhapticsGameConfig>();
	UObject* SettingAsset = gameConfig->Configuration.TryLoad();

	if (SettingAsset == NULL) 
	{
		return NULL;
	}

	UBhapticsSettings* Setting = Cast<UBhapticsSettings>(SettingAsset);

	if (Setting == NULL) {
		return NULL;
	}
	return Setting;
}


int UBhapticsSDK2::PlayHaptic(FString eventName)
{
	UBhapticsTactHubGameSubsystem::PlayHaptic(eventName);

	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::PlayHaptic(eventName);
}

int UBhapticsSDK2::PlayHapticWithOption(FString eventName, float intensity, float duration, float angleX, float offsetY)
{
	UBhapticsTactHubGameSubsystem::PlayHapticWithOptions(eventName, intensity, duration, angleX, offsetY);

	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::Play(eventName, intensity, duration, angleX, offsetY);
}

int UBhapticsSDK2::PlayDot(int position, TArray<int> motorValues, float seconds)
{
	
	UBhapticsTactHubGameSubsystem::PlayMotors(position, seconds * 1000, motorValues);

	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::PlayDot(position, seconds, motorValues);
}

int UBhapticsSDK2::PlayPath(int position, TArray<float> x, TArray<float> y, TArray<int> motorValues, float seconds)
{
	UBhapticsTactHubGameSubsystem::PlayPath(position, seconds * 1000, x, y, motorValues);
	
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::PlayPath(position, seconds, x, y, motorValues);
}

int UBhapticsSDK2::PlayWaveform(int position, TArray<int> motorIntensities, TArray<EBhapticsGlovePlayTime> playTimeValues, TArray<EBhapticsGloveShapeValue> shapeValues)
{
	UBhapticsTactHubGameSubsystem::PlayWaveform(position, playTimeValues, shapeValues, motorIntensities);
	
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::PlayWaveform(position, motorIntensities, playTimeValues, shapeValues);
}

int UBhapticsSDK2::PlayLoop(FString eventId, float intensity, float duration, float angleX, float offsetY, int interval, int maxCount)
{
	UBhapticsTactHubGameSubsystem::PlayLoopHapticWithOptions(eventId, intensity, duration, angleX, offsetY, interval, maxCount);
	
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return -1;
	}

	return BhapticsRequest::PlayLoop(eventId, intensity, duration, angleX, offsetY, interval, maxCount);
}

bool UBhapticsSDK2::StopHapticByEventId(FString eventId)
{
	UBhapticsTactHubGameSubsystem::StopByEventName(eventId);
	
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}

	return BhapticsRequest::StopByEventId(eventId);
}

bool UBhapticsSDK2::StopHapticByRequestId(int requestId)
{
	UBhapticsTactHubGameSubsystem::StopByRequestId(requestId);
	
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}

	return BhapticsRequest::StopByRequestId(requestId);
}

bool UBhapticsSDK2::StopHaptic()
{
	UBhapticsTactHubGameSubsystem::StopAll();
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}
	return BhapticsRequest::Stop();
}

void UBhapticsSDK2::Ping(FBhapticsDevice device)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return;
	}

	BhapticsRequest::Ping(device);
}

void UBhapticsSDK2::PingAll()
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return;
	}

	BhapticsRequest::PingAll();
}

void UBhapticsSDK2::SwapPosition(FBhapticsDevice device)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return;
	}

	BhapticsRequest::SwapPosition(device);
}

bool UBhapticsSDK2::IsPlaying()
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}
	return BhapticsRequest::IsPlaying();
}

bool UBhapticsSDK2::IsPlayingByRequestId(int requestId)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}

	return BhapticsRequest::IsPlayingByRequestId(requestId);
}

bool UBhapticsSDK2::IsPlayingByEventId(FString eventId)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}

	return BhapticsRequest::IsPlayingByEventId(eventId);
}


void UBhapticsSDK2::Destroy()
{
	BhapticsRequest::Destroy();
	Initialized = false;
	IsConfigurationFailed = false;
}

bool UBhapticsSDK2::IsBhapticsAvailable() 
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return false;
	}

	return Initialized && !IsConfigurationFailed;
}



TArray<FBhapticsDevice> UBhapticsSDK2::GetBhapticsDevices()
{
	if (!UBhapticsSDK2::TryInitializeAndCheck()) {
		return TArray<FBhapticsDevice>();
	}

	return BhapticsRequest::GetDevices();
}

FBhapticsRotationOption UBhapticsSDK2::ProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck())
	{
		return FBhapticsRotationOption(0, 0);
	}

	return BhapticsRequest::ProjectToVest(Location, HitComponent, HalfHeight);
}

FBhapticsRotationOption UBhapticsSDK2::ProjectToVestLocation(FVector ContactLocation, FVector PlayerLocation, FRotator PlayerRotation)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck())
	{
		return FBhapticsRotationOption(0, 0);
	}

	return BhapticsRequest::ProjectToVestLocation(ContactLocation, PlayerLocation, PlayerRotation);
}

FBhapticsRotationOption UBhapticsSDK2::CustomProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight, FVector UpVector, FVector ForwardVector)
{
	if (!UBhapticsSDK2::TryInitializeAndCheck())
	{
		return FBhapticsRotationOption(0, 0);
	}

	return BhapticsRequest::CustomProjectToVest(Location, HitComponent, HalfHeight, UpVector, ForwardVector);
}


