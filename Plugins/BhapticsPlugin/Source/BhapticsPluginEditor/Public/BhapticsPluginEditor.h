//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDetailCustomization.h"

#include "BhapticsSettings.h"
#include "BhapticsStructs.h"
#include "BhapticsGameConfig.h"
#include "BhapticsPluginEditorSettings.h"

#define Bhaptics_UE_VERSION_EQUALS_OR_BELOW(major, minor) (ENGINE_MAJOR_VERSION < (major) || (ENGINE_MAJOR_VERSION == (major) && ENGINE_MINOR_VERSION <= (minor)))

class FBhapticsPluginEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	void RegisterSettings();
	void UnregisterSettings();
private:

	void SyncSettingPathIfNeeded();
	void SyncEditorSettingIfNeeded();
	void SetStatusFailed();

	UBhapticsPluginEditorSettings* EditorSetting;
	UBhapticsGameConfig* GameConfig;
};


class FBhapticsDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
private:
	UBhapticsSettings* CrerateBhapticsSettings(int& Status);
	UBhapticsSettings* SaveBhapticsSettings(int& Status);

	TArray<FBhapticsMappingMetaData> GetMappings(int& Status);
	BhapticsDefaultConfig GetDefaultSetting(int& Status);

	FReply LinkSetting();
	FReply UnlinkSetting();

	void BuildConfigurationValidSection();
	void BuildConfigurationNotValidSection(int& status);

	FReply Check();


	IDetailLayoutBuilder* SavedLayoutBuilder;
	UBhapticsPluginEditorSettings* EditorSetting;
	UBhapticsGameConfig* GameConfig;
	UBhapticsSettings* Settings;
};