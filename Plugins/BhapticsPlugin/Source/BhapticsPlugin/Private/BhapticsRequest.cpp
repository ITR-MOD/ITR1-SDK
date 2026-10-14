//Copyright bHaptics Inc. All Rights Reserved 2022~


#include "BhapticsRequest.h"
#include "JsonObjectConverter.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#elif PLATFORM_WINDOWS
#include "BhapticsCPP/BhapticsCPP.h"
#endif 
#include <string>

#if PLATFORM_ANDROID
jmethodID PlayMethodId;
jmethodID PlayDotMethodId;
jmethodID PlayGloveMethodId;
jmethodID PlayPathMethodId;
jmethodID PlayLoopMethodId;
jmethodID InitializeMethodId;
jmethodID InitializePermissionMethodId;

jmethodID IsBhapticsAvailableMethodId;

jmethodID IsPlayingByRequestIdMethodId;
jmethodID IsPlayingByEventIdMethodId;
jmethodID IsPlayingMethodId;

jmethodID GetDevicesMethodId;
jmethodID PingMethodId;
jmethodID PingAllMethodId;
jmethodID SwapPositionMethodId;


jmethodID StopAllMethodId;
jmethodID StopByRequestIdMethodId;
jmethodID StopByEventIdMethodId;

void FindMethods() {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		PlayMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Play", "(Ljava/lang/String;FFFF)I", false);
		PlayDotMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_PlayDot", "(II[I)I", false);
		PlayGloveMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_PlayGlove", "(I[I[I[I)I", false);
		PlayPathMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_PlayPath", "(II[F[F[I)I", false);
		PlayLoopMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_PlayLoop", "(Ljava/lang/String;FFFFII)I", false);
		
		InitializeMethodId = FJavaWrapper::FindMethod(
			Env, FJavaWrapper::GameActivityClassID,
			"AndroidThunkJava_Initialize", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);

		InitializePermissionMethodId = FJavaWrapper::FindMethod(
			Env, FJavaWrapper::GameActivityClassID,
			"AndroidThunkJava_InitializeWithPermission", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V", false);

		IsBhapticsAvailableMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IsBhapticsAvailable", "()Z", false);

		IsPlayingByEventIdMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IsPlayingByEventId", "(Ljava/lang/String;)Z", false);
		IsPlayingByRequestIdMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IsPlayingByRequestId", "(I)Z", false);
		IsPlayingMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IsPlaying", "()Z", false);
		
		SwapPositionMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_SwapPosition", "(Ljava/lang/String;)V", false);
		PingMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Ping", "(Ljava/lang/String;)V", false);
		PingAllMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_PingAll", "()V", false);
		
		
		GetDevicesMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_getDeviceList", "()Ljava/lang/String;", false);
		

		StopAllMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_StopAll", "()Z", false);
		StopByRequestIdMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_StopByRequestId", "(I)Z", false);
		StopByEventIdMethodId =
			FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_StopByEventId", "(Ljava/lang/String;)Z", false);
	}
}
#endif


bool isBhapticsAvailable = false;
bool isBhapticsAvailableChecked = false;

int64 lastDeviceListCheckTime = 0;
TArray<FBhapticsDevice> DeviceList;


#if PLATFORM_ANDROID
JNI_METHOD void Java_com_epicgames_ue4_GameActivity_onRefreshPairedInfo(JNIEnv* jenv, jobject thiz)
{
	isBhapticsAvailableChecked = false;
}

JNI_METHOD void Java_com_epicgames_unreal_GameActivity_onRefreshPairedInfo(JNIEnv* jenv, jobject thiz)
{
	isBhapticsAvailableChecked = false;
}
#endif

void BhapticsRequest::Initialize(FString app, FString key, FString defaultConfig, bool bAutoLaunchPlayer)
{
#if PLATFORM_ANDROID
	FindMethods();
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring appStrJava = Env->NewStringUTF(TCHAR_TO_UTF8(*app));
		jstring keyStrJava = Env->NewStringUTF(TCHAR_TO_UTF8(*key));
		jstring defaultConfigJava = Env->NewStringUTF(TCHAR_TO_UTF8(*defaultConfig));

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitializeMethodId, appStrJava, keyStrJava, defaultConfigJava);

		Env->DeleteLocalRef(appStrJava);
		Env->DeleteLocalRef(keyStrJava);
		Env->DeleteLocalRef(defaultConfigJava);
	}

#elif PLATFORM_WINDOWS
	std::string appStr(TCHAR_TO_UTF8(*app));
	std::string keyStr(TCHAR_TO_UTF8(*key));
	std::string defaultConfigStr(TCHAR_TO_UTF8(*defaultConfig));

	registryAndInit(keyStr.c_str(), appStr.c_str(), defaultConfigStr.c_str());
#endif

	BhapticsRequest::IsBhapticsAvailable(bAutoLaunchPlayer);
}


void BhapticsRequest::InitializePermissionOption(FString app, FString key, FString defaultConfig, bool requestPermission, bool bAutoLaunchPlayer)
{
	UE_LOG(LogTemp, Log, TEXT("BhapticsRequest:InitializePermissionOption %s"), *app);
	
#if PLATFORM_ANDROID
	FindMethods();
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring appStrJava = Env->NewStringUTF(TCHAR_TO_UTF8(*app));
		jstring keyStrJava = Env->NewStringUTF(TCHAR_TO_UTF8(*key));
		jstring defaultConfigJava = Env->NewStringUTF(TCHAR_TO_UTF8(*defaultConfig));

		UE_LOG(LogTemp, Log, TEXT("BhapticsRequest::Initialize Android %s"), *app);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitializePermissionMethodId, appStrJava, keyStrJava, defaultConfigJava, requestPermission);

		Env->DeleteLocalRef(appStrJava);
		Env->DeleteLocalRef(keyStrJava);
		Env->DeleteLocalRef(defaultConfigJava);
	}

#elif PLATFORM_WINDOWS
	std::string appStr(TCHAR_TO_UTF8(*app));
	std::string keyStr(TCHAR_TO_UTF8(*key));
	std::string defaultConfigStr(TCHAR_TO_UTF8(*defaultConfig));
	
	registryAndInit(keyStr.c_str(), appStr.c_str(), defaultConfigStr.c_str());
#endif

	BhapticsRequest::IsBhapticsAvailable(bAutoLaunchPlayer);
}


bool BhapticsRequest::IsBhapticsAvailable(bool bAutoLaunchPlayer)
{
	if (isBhapticsAvailableChecked)
	{
		return isBhapticsAvailable;
	}


#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		isBhapticsAvailable = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, IsBhapticsAvailableMethodId);
		isBhapticsAvailableChecked = true;
	}

	return isBhapticsAvailable;

#elif PLATFORM_WINDOWS
	if (!isPlayerInstalled()) 
	{
		isBhapticsAvailable = false;
		isBhapticsAvailableChecked = true;
		return isBhapticsAvailable;
	}

	if (!isPlayerRunning())
	{
		launchPlayer(bAutoLaunchPlayer);
	}
#endif
	isBhapticsAvailable = true;
	isBhapticsAvailableChecked = true;
	return false;
}

int BhapticsRequest::PlayHaptic(FString eventName)
{
	return BhapticsRequest::Play(eventName, 1, 1, 0, 0);
}

int BhapticsRequest::Play(FString eventId, float intensity, float duration, float angleX, float offsetY)
{
	if (!isBhapticsAvailable)
	{
		return -1;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring eventIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*eventId));
		int res = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, PlayMethodId, eventIdJava, intensity, duration, angleX, offsetY);
		Env->DeleteLocalRef(eventIdJava);
		return res;
	}
	return -1;
#elif PLATFORM_WINDOWS
	std::string eventNameStr(TCHAR_TO_UTF8(*eventId));
	return playPosParam(eventNameStr.c_str(), 0, intensity, duration, angleX, offsetY);
#endif

	return -1;
}

int BhapticsRequest::PlayDot(int position, float duration, TArray<int> motorValues)
{
	if (!isBhapticsAvailable)
	{
		return -1;
	}

	int durationInt = (int)(duration * 1000);

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jintArray motorValuesJava = Env->NewIntArray(motorValues.Num());
		jint* indexes = new jint[motorValues.Num()];
		for (int i = 0; i < motorValues.Num(); ++i) {
			indexes[i] = FMath::Clamp(motorValues[i], 0, 100);
		}
		Env->SetIntArrayRegion(motorValuesJava, 0, motorValues.Num(), indexes);

		int res = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, PlayDotMethodId, position, durationInt, motorValuesJava);
		return res;
	}
	return -1;

#elif PLATFORM_WINDOWS
	int* motors = new int[motorValues.Num()];
	for (int i = 0; i < motorValues.Num(); ++i) {
		motors[i] = FMath::Clamp(motorValues[i], 0, 100);
	}
	return playDot(position, durationInt, motors, motorValues.Num());

#endif

	return -1;
}

int BhapticsRequest::PlayPath(
	int Position, const float DurationSeconds,
	const TArray<float>& X,
	const TArray<float>& Y,
	const TArray<int>& Intensities)
{
	if (!isBhapticsAvailable)
	{
		return -1;
	}

	
	if (X.Num() != Y.Num() || Intensities.Num() != X.Num()) 
	{
		UE_LOG(LogTemp, Error, TEXT("BhapticsRequest::PlayPath  - 'x, y, motorValues' necessarily require same size."));
		return -1;
	}

	int durationMillis = (int)(DurationSeconds * 1000);
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jintArray motorIntensitiesJava = Env->NewIntArray(Intensities.Num());
		jfloatArray xValuesJava = Env->NewFloatArray(X.Num());
		jfloatArray yValuesJava = Env->NewFloatArray(Y.Num());

		jint* motors = new jint[Intensities.Num()];
		for (int i = 0; i < Intensities.Num(); ++i) {
			motors[i] = FMath::Clamp(Intensities[i], 0, 100);
		}

		jfloat* x = new jfloat[X.Num()];
		jfloat* y = new jfloat[Y.Num()];
		for (int i = 0; i < X.Num(); ++i) {
			x[i] = X[i];
		}

		for (int i = 0; i < Y.Num(); ++i) {
			y[i] = Y[i];
		}


		Env->SetIntArrayRegion(motorIntensitiesJava, 0, Intensities.Num(), motors);
		Env->SetFloatArrayRegion(xValuesJava, 0, X.Num(), x);
		Env->SetFloatArrayRegion(yValuesJava, 0, Y.Num(), y);

		int res = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis,
			PlayPathMethodId, Position, durationMillis,
			xValuesJava, yValuesJava, motorIntensitiesJava);
		return res;
	}
	return -1;

#elif PLATFORM_WINDOWS
	int* motors = new int[Intensities.Num()];
	for (int i = 0; i < Intensities.Num(); ++i) {
		motors[i] = FMath::Clamp(Intensities[i], 0, 100);
	}
	float* x = new float[X.Num()];
	for (int i = 0; i < X.Num(); ++i) {
		x[i] = x[i];
	}
	float* y = new float[Y.Num()];
	for (int i = 0; i < Y.Num(); ++i) {
		y[i] = Y[i];
	}
	return playPath(Position, durationMillis, x, y, motors, Intensities.Num());
#else
	return -1;
#endif
}

int BhapticsRequest::PlayWaveform(int position, TArray<int> motorIntensities, TArray<EBhapticsGlovePlayTime> playTimeValues, TArray<EBhapticsGloveShapeValue> shapeValues)
{
	if (!isBhapticsAvailable)
	{
		return -1;
	}

	if (motorIntensities.Num() != 6 || playTimeValues.Num() != 6 || shapeValues.Num() != 6) 
	{
		UE_LOG(LogTemp, Error, TEXT("BhapticsRequest::PlayGlove  - 'motorValues, playTimeValues, shapeValues' necessarily require 6 values each."));
		return -1;
	}


#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jintArray motorIntensitiesJava = Env->NewIntArray(motorIntensities.Num());
		jintArray playTimeValuesJava = Env->NewIntArray(playTimeValues.Num());
		jintArray shapeValuesJava = Env->NewIntArray(shapeValues.Num());

		jint* motors = new jint[motorIntensities.Num()];
		for (int i = 0; i < motorIntensities.Num(); ++i) {
			motors[i] = FMath::Clamp(motorIntensities[i], 0, 100);
		}

		jint* playTimes = new jint[playTimeValues.Num()];
		jint* tempShapeValues = new jint[shapeValues.Num()];
		for (int i = 0; i < playTimeValues.Num(); ++i) {
			playTimes[i] = (int)playTimeValues[i];
		}

		for (int i = 0; i < shapeValues.Num(); ++i) {
			tempShapeValues[i] = (int)shapeValues[i];
		}


		Env->SetIntArrayRegion(motorIntensitiesJava, 0, motorIntensities.Num(), motors);
		Env->SetIntArrayRegion(playTimeValuesJava, 0, playTimeValues.Num(), playTimes);
		Env->SetIntArrayRegion(shapeValuesJava, 0, shapeValues.Num(), tempShapeValues);

		int res = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, PlayGloveMethodId, position, motorIntensitiesJava, playTimeValuesJava, shapeValuesJava);
		return res;
	}
	return -1;

#elif PLATFORM_WINDOWS
	int* motors = new int[motorIntensities.Num()];
	for (int i = 0; i < motorIntensities.Num(); ++i) {
		motors[i] = FMath::Clamp(motorIntensities[i], 0, 100);
	}
	int* playTimes = new int[playTimeValues.Num()];
	for (int i = 0; i < playTimeValues.Num(); ++i) {
		playTimes[i] = (int)playTimeValues[i];
	}
	int* tempShapeValues = new int[shapeValues.Num()];
	for (int i = 0; i < shapeValues.Num(); ++i) {
		tempShapeValues[i] = (int)shapeValues[i];
	}
	return playWaveform(position, motors, playTimes, tempShapeValues, 1, 6);
#endif

	return -1;
}

int BhapticsRequest::PlayLoop(FString eventId, float intensity, float duration, float angleX, float offsetY, int interval, int maxCount)
{
	if (!isBhapticsAvailable)
	{
		return -1;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring eventIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*eventId));
		int res = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, PlayLoopMethodId, eventIdJava, intensity, duration, angleX, offsetY, interval, maxCount);
		Env->DeleteLocalRef(eventIdJava);
		return res;
	}
	return -1;
#elif PLATFORM_WINDOWS
	std::string eventNameStr(TCHAR_TO_UTF8(*eventId));
	return playLoop(eventNameStr.c_str(), intensity, duration, angleX, offsetY, interval, maxCount);
#endif

	return -1;
}

bool BhapticsRequest::IsPlaying()
{
	if (!isBhapticsAvailable)
	{
		return false;
	}


#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, IsPlayingMethodId);
		return res;
	}
	return false;
#elif PLATFORM_WINDOWS
	return isPlaying();
#else
	return false;
#endif
}

bool BhapticsRequest::IsPlayingByRequestId(int requestId)
{
	if (!isBhapticsAvailable)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, IsPlayingByRequestIdMethodId, requestId);
		return res;
	}
	return false;
#elif PLATFORM_WINDOWS
	return isPlayingByRequestId(requestId);
#else
	return false;
#endif
}

bool BhapticsRequest::IsPlayingByEventId(FString eventId)
{
	if (!isBhapticsAvailable)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring eventIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*eventId));
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, IsPlayingByEventIdMethodId, eventIdJava);
		Env->DeleteLocalRef(eventIdJava);
		return res;
	}

	return false;
#elif PLATFORM_WINDOWS
	std::string eventIdStr(TCHAR_TO_UTF8(*eventId));
	return isPlayingByEventId(eventIdStr.c_str());
#endif

	return false;
}

TArray<FBhapticsDevice> BhapticsRequest::GetDevices()
{
	if (!isBhapticsAvailable)
	{
		return TArray<FBhapticsDevice>();
	}

	FDateTime Time = FDateTime::Now();
	int64 Timestamp = (int64)(Time.GetTicks() / ETimespan::TicksPerMillisecond);
	if (Timestamp - lastDeviceListCheckTime < 1000) {
		return DeviceList;
	}

	lastDeviceListCheckTime = Timestamp;

#if PLATFORM_ANDROID

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jstr = (jstring) FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, GetDevicesMethodId);
		const char* nativeDeviceListString = Env->GetStringUTFChars(jstr, 0);
		FString DevicesListString = FString(nativeDeviceListString);

		Env->ReleaseStringUTFChars(jstr, nativeDeviceListString);

		if (!FJsonObjectConverter::JsonArrayStringToUStruct(DevicesListString, &DeviceList, 0, 0))
		{
			return TArray<FBhapticsDevice>();
		}
		return DeviceList;
	}
	return TArray<FBhapticsDevice>();


#elif PLATFORM_WINDOWS
	std::string deviceStr = getDeviceInfoJson();

	if (!wsIsConnected()) {
		DeviceList = TArray<FBhapticsDevice>();
		return TArray<FBhapticsDevice>();
	}


	FString deviceFString(deviceStr.c_str());

	if (!FJsonObjectConverter::JsonArrayStringToUStruct(deviceFString, &DeviceList, 0, 0))
	{
		return TArray<FBhapticsDevice>();
	}
#endif
	return DeviceList;
}

void BhapticsRequest::Ping(FBhapticsDevice device)
{
	if (!isBhapticsAvailable)
	{
		return;
	}

	std::string addrStr(TCHAR_TO_UTF8(*device.Address));

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring deviceIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*device.Address));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, PingMethodId, deviceIdJava);
		Env->DeleteLocalRef(deviceIdJava);
	}

#elif PLATFORM_WINDOWS
	ping(addrStr.c_str());
#endif
}

void BhapticsRequest::PingAll()
{
	if (!isBhapticsAvailable)
	{
		return;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, PingAllMethodId);
	}

#elif PLATFORM_WINDOWS
	pingAll();
#endif
}

void BhapticsRequest::SwapPosition(FBhapticsDevice device)
{
	if (!isBhapticsAvailable)
	{
		return;
	}

	std::string addrStr(TCHAR_TO_UTF8(*device.Address));

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring deviceIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*device.Address));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, SwapPositionMethodId, deviceIdJava);
		Env->DeleteLocalRef(deviceIdJava);
	}
#elif PLATFORM_WINDOWS
	swapPosition(addrStr.c_str());
#endif
}

bool BhapticsRequest::StopByEventId(FString eventId)
{
	if (!isBhapticsAvailable)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring eventIdJava = Env->NewStringUTF(TCHAR_TO_UTF8(*eventId));
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, StopByEventIdMethodId, eventIdJava);
		Env->DeleteLocalRef(eventIdJava);
		return res;
	}
	return false;
#elif PLATFORM_WINDOWS
	std::string eventIdStr(TCHAR_TO_UTF8(*eventId));

	return stopByEventId(eventIdStr.c_str());
#endif

	return false;
}

bool BhapticsRequest::StopByRequestId(int requestId)
{
	if (!isBhapticsAvailable)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, StopByRequestIdMethodId, requestId);
		return res;
	}


	return false;
#elif PLATFORM_WINDOWS
	return stop(requestId);
#else
	return false;
#endif
}

bool BhapticsRequest::Stop()
{
	if (!isBhapticsAvailable)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool res = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, StopAllMethodId);
		return res;
	}
	return false;
#elif PLATFORM_WINDOWS
	return stopAll();
#else
	return false;
#endif
}

void BhapticsRequest::Destroy()
{
#if PLATFORM_ANDROID
	// nothing to do
#elif PLATFORM_WINDOWS
	wsClose();
#endif
}



FBhapticsRotationOption BhapticsRequest::ProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight) 
{
	if (HitComponent == nullptr)
	{
		return FBhapticsRotationOption(0, 0);
	}

	const FRotator InverseRotation = HitComponent->GetComponentRotation().GetInverse();
	FVector HitPoint = InverseRotation.RotateVector(Location - HitComponent->GetComponentLocation());
	FVector UpVector = FVector::UpVector;//InverseRotation.RotateVector(HitComponent->GetUpVector());
	FVector ForwardVector = FVector::ForwardVector;//InverseRotation.RotateVector(HitComponent->GetForwardVector());
	const FVector Scale = HitComponent->GetComponentScale();
	float Y_Offset;

	UpVector.Normalize();
	ForwardVector.Normalize();

	HitPoint.X = HitPoint.X / Scale.X;
	HitPoint.Y = HitPoint.Y / Scale.Y;
	HitPoint.Z = HitPoint.Z / Scale.Z;

	const float DotProduct = FVector::DotProduct(HitPoint, UpVector);

	FVector Result = HitPoint - (DotProduct * UpVector);
	Result.Normalize();

	const float A = Result.X * ForwardVector.Y - ForwardVector.X * Result.Y;
	const float B = ForwardVector.X * Result.X + Result.Y * ForwardVector.Y;

	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(A, B));

	if (HalfHeight < 0.01)
	{
		Y_Offset = 0;
	}
	else
	{
		Y_Offset = FMath::Clamp(DotProduct / (HalfHeight * 2), -0.5f, 0.5f);
	}

	return FBhapticsRotationOption(Angle, Y_Offset);
}

FBhapticsRotationOption BhapticsRequest::ProjectToVestLocation(FVector ContactLocation, FVector PlayerLocation, FRotator PlayerRotation)
{
	const FRotator InverseRotation = PlayerRotation.GetInverse();
	const FVector HitPoint = InverseRotation.RotateVector(ContactLocation - PlayerLocation);
	FVector UpVector = FVector::UpVector;//InverseRotation.RotateVector(HitComponent->GetUpVector());
	FVector ForwardVector = FVector::ForwardVector;//InverseRotation.RotateVector(HitComponent->GetForwardVector());

	UpVector.Normalize();
	ForwardVector.Normalize();

	const float DotProduct = FVector::DotProduct(HitPoint, UpVector);

	FVector Result = HitPoint - (DotProduct * UpVector);
	Result.Normalize();

	const float A = Result.X * ForwardVector.Y - ForwardVector.X * Result.Y;
	const float B = ForwardVector.X * Result.X + Result.Y * ForwardVector.Y;

	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(A, B));

	return FBhapticsRotationOption(Angle, 0);
}

FBhapticsRotationOption BhapticsRequest::CustomProjectToVest(FVector Location, UPrimitiveComponent* HitComponent, float HalfHeight, FVector UpVector, FVector ForwardVector)
{
	if (HitComponent == nullptr)
	{
		return FBhapticsRotationOption(0, 0);
	}

	const FRotator InverseRotation = HitComponent->GetComponentRotation().GetInverse();
	FVector HitPoint = InverseRotation.RotateVector(Location - HitComponent->GetComponentLocation());
	const FVector Scale = HitComponent->GetComponentScale();

	if (UpVector == FVector::ZeroVector)
	{
		UpVector = InverseRotation.RotateVector(HitComponent->GetUpVector());
	}
	else
	{
		UpVector = InverseRotation.RotateVector(UpVector);
	}

	if (ForwardVector == FVector::ZeroVector)
	{
		ForwardVector = InverseRotation.RotateVector(HitComponent->GetForwardVector());
	}
	else
	{
		ForwardVector = InverseRotation.RotateVector(ForwardVector);
	}

	UpVector.Normalize();
	ForwardVector.Normalize();

	HitPoint.X = HitPoint.X / Scale.X;
	HitPoint.Y = HitPoint.Y / Scale.Y;
	HitPoint.Z = HitPoint.Z / Scale.Z;

	const float DotProduct = FVector::DotProduct(HitPoint, UpVector);

	FVector Result = HitPoint - (DotProduct * UpVector);
	Result.Normalize();

	const float A = Result.X * ForwardVector.Y - ForwardVector.X * Result.Y + Result.Y * ForwardVector.Z - ForwardVector.Y *
		Result.Z + Result.Z * ForwardVector.X - ForwardVector.Z * Result.X;
	const float B = ForwardVector.X * Result.X + Result.Y * ForwardVector.Y + Result.Z * ForwardVector.Z;

	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(A, B));

	const float Y_Offset = FMath::Clamp(DotProduct / (HalfHeight * 2), -0.5f, 0.5f);

	return FBhapticsRotationOption(Angle, Y_Offset);
}
