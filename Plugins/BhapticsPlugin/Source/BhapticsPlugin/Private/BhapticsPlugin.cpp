//Copyright bHaptics Inc. All Rights Reserved 2022~

#include "BhapticsPlugin.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC
#include "BhapticsCPP/BhapticsCPP.h"

#endif

#include "BhapticsRequest.h"
#include "BhapticsSettings.h"

#define LOCTEXT_NAMESPACE "FBhapticsPluginModule"

DEFINE_LOG_CATEGORY_STATIC(BhapticsPlugin, Log, All);

void FBhapticsPluginModule::StartupModule()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC
	const auto bRes = LoadDll();
	if (!bRes) {
		return;
	}
#endif
}

bool FBhapticsPluginModule::LoadDll() {
	FString BaseDir = IPluginManager::Get().FindPlugin("BhapticsPlugin")->GetBaseDir();
	FString BhapticsLibraryPath;

#if PLATFORM_WINDOWS
	BhapticsLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/BhapticsCPP/Win64/bhaptics_library.dll"));
#elif PLATFORM_MAC
	BhapticsLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/BhapticsCPP/Mac/bhaptics_library.dylib"));
#endif

	BhapticsLibraryHandle = !BhapticsLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*BhapticsLibraryPath) : nullptr;


	if (BhapticsLibraryHandle == nullptr)
	{
		UE_LOG(BhapticsPlugin, Log, TEXT("UBhapticsSDK2:: Failed to load bhaptics_library.dll"));
		return false;
	}

	return true;

}

void FBhapticsPluginModule::ShutdownModule()
{
	if (BhapticsLibraryHandle != nullptr) {
		BhapticsRequest::Destroy();
		
		FPlatformProcess::FreeDllHandle(BhapticsLibraryHandle);
	}
	BhapticsLibraryHandle = nullptr;


}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBhapticsPluginModule, BhapticsPlugin)
