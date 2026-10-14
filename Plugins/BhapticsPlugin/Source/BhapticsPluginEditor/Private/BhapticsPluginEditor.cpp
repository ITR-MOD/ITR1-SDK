//Copyright bHaptics Inc. All Rights Reserved 2022~

#include "BhapticsPluginEditor.h"

#include "BhapticsSDK2.h" 

#include "ISettingsModule.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"

#include "Widgets/Input/SButton.h"
#include "DetailWidgetRow.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#if !Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
#include "UObject/SavePackage.h"
#endif
#include "BhapticsCPP/BhapticsCPP.h"
#include <string>
#include "JsonObjectConverter.h"

#include "EditorStyleSet.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FBhapticsPluginEditorModule"

void FBhapticsPluginEditorModule::StartupModule()
{
	EditorSetting = GetMutableDefault<UBhapticsPluginEditorSettings>();
	GameConfig = GetMutableDefault<UBhapticsGameConfig>();

	SyncEditorSettingIfNeeded();
	SyncSettingPathIfNeeded();

	RegisterSettings();
}

void FBhapticsPluginEditorModule::ShutdownModule()
{
	UnregisterSettings();
}

void FBhapticsPluginEditorModule::SyncEditorSettingIfNeeded()
{
	UObject* SettingAsset = EditorSetting->Setting.TryLoad();

	if (SettingAsset == NULL) {
		SetStatusFailed();
		return;
	}

	UBhapticsSettings* Setting = Cast<UBhapticsSettings>(SettingAsset);
	if (Setting == NULL) {
		SetStatusFailed();
		return;
	}

	if (!EditorSetting->ApiKey.Equals(Setting->ApiKey) ||
		!EditorSetting->AppId.Equals(Setting->AppId))
	{
		EditorSetting->ApiKey = Setting->ApiKey;
		EditorSetting->AppId = Setting->AppId;
		EditorSetting->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
		EditorSetting->UpdateDefaultConfigFile();
#else
		EditorSetting->TryUpdateDefaultConfigFile();
#endif
		Setting = NULL;
		return;
	}
	Setting = NULL;
}

void FBhapticsPluginEditorModule::SyncSettingPathIfNeeded()
{
	auto name = GameConfig->Configuration.GetLongPackageName();

	bool isPathDifferent;
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5, 0)
	isPathDifferent = !EditorSetting->Setting.GetAssetPathName().IsEqual(GameConfig->Configuration.GetAssetPathName());
#else
	isPathDifferent = EditorSetting->Setting.GetAssetPath() != GameConfig->Configuration.GetAssetPath();
#endif

	if (isPathDifferent)
	{
		GameConfig->Configuration.Reset();
		GameConfig->Configuration = EditorSetting->Setting;
		GameConfig->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
		GameConfig->UpdateDefaultConfigFile();
#else
		GameConfig->TryUpdateDefaultConfigFile();
#endif
	}
}

void FBhapticsPluginEditorModule::SetStatusFailed()
{
	EditorSetting->Status = -1;
	EditorSetting->Setting.Reset();
	EditorSetting->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	EditorSetting->UpdateDefaultConfigFile();
#else
	EditorSetting->TryUpdateDefaultConfigFile();
#endif

	GameConfig->Configuration.Reset();
	GameConfig->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	GameConfig->UpdateDefaultConfigFile();
#else
	GameConfig->TryUpdateDefaultConfigFile();
#endif
}

void FBhapticsPluginEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "bHaptics",
			LOCTEXT("BhapticsPluginEditorSettingsName", "bHaptics"),
			LOCTEXT("BhapticsPluginEditorSettingsDescription", "Configure the bHaptics plugin"),
			GetMutableDefault<UBhapticsPluginEditorSettings>()
		);

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(
			UBhapticsPluginEditorSettings::StaticClass()->GetFName(), 
			FOnGetDetailCustomizationInstance::CreateStatic(&FBhapticsDetailsCustomization::MakeInstance)
		);
	}
}

void FBhapticsPluginEditorModule::UnregisterSettings()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "bHaptics");
	}
}


IMPLEMENT_MODULE(FBhapticsPluginEditorModule, BhapticsPluginEditor)

TSharedRef<IDetailCustomization> FBhapticsDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FBhapticsDetailsCustomization);
}

void FBhapticsDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	SavedLayoutBuilder = &DetailLayout;
	EditorSetting = GetMutableDefault<UBhapticsPluginEditorSettings>();
	GameConfig = GetMutableDefault<UBhapticsGameConfig>();

	int status = EditorSetting->Status;

	Settings = UBhapticsSDK2::TryLoadBhapticsConfig();

	if (Settings == NULL) {
		BuildConfigurationNotValidSection(status);
		return;
	}

	BuildConfigurationValidSection();
}

FReply FBhapticsDetailsCustomization::Check() {
	auto path = Settings->GetPathName();

	GameConfig->Configuration.SetPath(path);
	GameConfig->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	GameConfig->UpdateDefaultConfigFile();
#else
	GameConfig->TryUpdateDefaultConfigFile();
#endif

	EditorSetting->Setting.SetPath(path);
	EditorSetting->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	EditorSetting->UpdateDefaultConfigFile();
#else
	EditorSetting->TryUpdateDefaultConfigFile();
#endif

	return FReply::Handled();
}

FReply FBhapticsDetailsCustomization::LinkSetting()
{
	//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::LinkSetting"));

	int status = -1;

	if (Settings == NULL) {
		Settings = CrerateBhapticsSettings(status);
	}
	else {
		Settings = SaveBhapticsSettings(status);
	}


	// TODO setting validation
	if (IsValid(Settings) && status == 0) {
		GameConfig->Configuration = Settings->GetPathName();
		GameConfig->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
		GameConfig->UpdateDefaultConfigFile();
#else
		GameConfig->TryUpdateDefaultConfigFile();
#endif
	}

	EditorSetting->Status = status;
	EditorSetting->Setting = Settings->GetPathName();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	EditorSetting->UpdateDefaultConfigFile();
#else
	EditorSetting->TryUpdateDefaultConfigFile();
#endif
	SavedLayoutBuilder->ForceRefreshDetails();

	return FReply::Handled();
}

FReply FBhapticsDetailsCustomization::UnlinkSetting()
{
	//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::UnlinkSetting"));

	EditorSetting->ApiKey = TEXT("");
	EditorSetting->AppId = TEXT("");
	EditorSetting->Status = -1;
	EditorSetting->Setting.Reset();
	EditorSetting->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	EditorSetting->UpdateDefaultConfigFile();
#else
	EditorSetting->TryUpdateDefaultConfigFile();
#endif

	GameConfig->Configuration.Reset();
	GameConfig->MarkPackageDirty();
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	GameConfig->UpdateDefaultConfigFile();
#else
	GameConfig->TryUpdateDefaultConfigFile();
#endif

	SavedLayoutBuilder->ForceRefreshDetails();

	return FReply::Handled();
}

UBhapticsSettings* FBhapticsDetailsCustomization::SaveBhapticsSettings(int& status) {

	FString PackageName = GameConfig->Configuration.GetLongPackageName(); // TEXT("/Game/Bhaptics/");

	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();
	Settings->AppId = EditorSetting->AppId;
	Settings->ApiKey = EditorSetting->ApiKey;
	Settings->EventList = GetMappings(status);

	if (status != 0) {
		//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::GetMappings %d"), status);
		return NULL;
	}
	auto Config = GetDefaultSetting(status);
	if (status != 0) {
		//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::GetDefaultSetting %d"), status);
		return NULL;
	}
	Settings->DefaultConfig = Config.ConfigString;
	Settings->AutoLaunchPlayer = Config.bAutoLaunchPlayer;
	Settings->AppName = Config.Name;
	Settings->LastDeployVersion = Config.Version;

	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(Settings);


	FString PackageFileName = FPackageName::LongPackageNameToFilename(
		PackageName, FPackageName::GetAssetPackageExtension());

	bool bSaved;

#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	bSaved = UPackage::SavePackage(
		Package,
		Settings,
		EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
		*PackageFileName,
		GError, nullptr, true, true, SAVE_NoError);
#else
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.bForceByteSwapping = true;
	SaveArgs.bWarnOfLongFilename = true;
	SaveArgs.SaveFlags = SAVE_NoError;

	bSaved = UPackage::SavePackage(Package, Settings, *PackageFileName, SaveArgs);
#endif

	TArray<UObject*> ObjectsToSync;
	ObjectsToSync.Add(Settings);
	GEditor->SyncBrowserToObjects(ObjectsToSync);

	return Settings;
}

UBhapticsSettings* FBhapticsDetailsCustomization::CrerateBhapticsSettings(int& status)
{
	FString AssetName = TEXT("BhapticsSettings");
	FString PackageName = TEXT("/Game/Bhaptics/");
	PackageName += AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();

	Settings = NewObject<UBhapticsSettings>(
		Package, *AssetName, RF_Public | RF_Standalone | RF_MarkAsRootSet);

	Settings->AppId = EditorSetting->AppId;
	Settings->ApiKey = EditorSetting->ApiKey;
	Settings->EventList = GetMappings(status);

	if (status != 0) {
		//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::GetMappings %d"), status);
		return NULL;
	}
	auto Config = GetDefaultSetting(status);
	if (status != 0) {
		//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::GetDefaultSetting %d"), status);
		return NULL;
	}
	Settings->DefaultConfig = Config.ConfigString;
	Settings->AppName = Config.Name;
	Settings->LastDeployVersion = Config.Version;
	Settings->AutoLaunchPlayer = Config.bAutoLaunchPlayer;

	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(Settings);


	FString PackageFileName = FPackageName::LongPackageNameToFilename(
		PackageName, FPackageName::GetAssetPackageExtension());

	bool bSaved;

#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
	bSaved = UPackage::SavePackage(
		Package,
		Settings,
		EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
		*PackageFileName,
		GError, nullptr, true, true, SAVE_NoError);
#else
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.Error = GError;
	SaveArgs.bForceByteSwapping = true;
	SaveArgs.bWarnOfLongFilename = true;
	SaveArgs.SaveFlags = SAVE_NoError;

	bSaved = UPackage::SavePackage(Package, Settings, *PackageFileName, SaveArgs);
#endif

	TArray<UObject*> ObjectsToSync;
	ObjectsToSync.Add(Settings);
	GEditor->SyncBrowserToObjects(ObjectsToSync);

	return Settings;
}


TArray<FBhapticsMappingMetaData> FBhapticsDetailsCustomization::GetMappings(int& status)
{
	std::string appStr(TCHAR_TO_UTF8(*Settings->AppId));
	std::string keyStr(TCHAR_TO_UTF8(*Settings->ApiKey));
	std::string mappings = bHapticsGetHapticMappings(keyStr.c_str(), appStr.c_str(), -1, status);

	FString eventSettingsString(mappings.c_str());

	//UE_LOG(LogTemp, Log, TEXT("FBhapticsDetailsCustomization::bHapticsGetHapticMappings %s"), *eventSettingsString);
	FBhapticsEventListMessage message;
	TArray<FBhapticsMappingMetaData> EventList;

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(eventSettingsString, &message, 0, 0))
	{
		//UE_LOG(LogTemp, Log, TEXT("parse failed"));
		return  TArray<FBhapticsMappingMetaData>();
	}
	if (!message.Status) {
		return TArray<FBhapticsMappingMetaData>();
	}

	return message.Message;
	
}

BhapticsDefaultConfig FBhapticsDetailsCustomization::GetDefaultSetting(int& status)
{
	std::string appStr(TCHAR_TO_UTF8(*Settings->AppId));
	std::string keyStr(TCHAR_TO_UTF8(*Settings->ApiKey));
	std::string mappings = bHapticsGetHapticMessage(keyStr.c_str(), appStr.c_str(), -1, status);
	FString eventSettingsString(mappings.c_str());

	BhapticsDefaultConfig Config;
	Config.ConfigString = eventSettingsString;

	FBhapticsDefaultConfigMessage message;

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(eventSettingsString, &message, 0, 0))
	{
		//UE_LOG(LogTemp, Log, TEXT("parse failed"));
		status = 10;
		return Config;
	}
	if (!message.Status) {
		return Config;
	}

	Config.bAutoLaunchPlayer = message.Message.AutoLaunchPlayer;
	Config.Version = message.Message.Version;
	Config.Name = message.Message.Name;

	return Config;
}

void FBhapticsDetailsCustomization::BuildConfigurationValidSection()
{
	FText StateMessage = FText::FromString("BHAPTICS_SETTINGS_SUCCESS");
	IDetailCategoryBuilder& CategoryBuilder = SavedLayoutBuilder->EditCategory("bHaptics Setting", FText::GetEmpty());
	TSharedRef<SHorizontalBox> Result = SNew(SHorizontalBox)
		.ToolTipText(FText::GetEmpty())
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
			SNew(SImage)
			.Image(FEditorStyle::GetBrush(FName("SettingsEditor.GoodIcon")))
#else
			SNew(SImage)
			.Image(FAppStyle::GetBrush(FName("SettingsEditor.GoodIcon")))
#endif
		]

	// Notice
	+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(16.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::Green)
			.ShadowColorAndOpacity(FLinearColor::Black)
			.ShadowOffset(FVector2D::UnitVector)
			.Text(StateMessage)
		]
	+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth().Padding(2, 0.0f)
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor::Green)
			.Text(LOCTEXT("Update bHaptics", "Update bHaptics"))
			.VAlign(VAlign_Center)
			.OnClicked_Raw(this, &FBhapticsDetailsCustomization::LinkSetting)
		]
	//+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth()
	//	[
	//		SNew(SButton)
	//		.ButtonColorAndOpacity(FLinearColor::Red)
	//	.Text(LOCTEXT("Check", "Check"))
	//	.VAlign(VAlign_Center)
	//	.OnClicked_Raw(this, &FBhapticsDetailsCustomization::Check)
	//	]
	+ SHorizontalBox::Slot().HAlign(HAlign_Right).AutoWidth()
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor::Red)
			.Text(LOCTEXT("Unlink bHaptics", "Unlink bHaptics"))
			.VAlign(VAlign_Center)
			.OnClicked_Raw(this, &FBhapticsDetailsCustomization::UnlinkSetting)
		];

	CategoryBuilder.AddCustomRow(LOCTEXT("", ""), false).WholeRowWidget[
		Result
	];
}

void FBhapticsDetailsCustomization::BuildConfigurationNotValidSection(int& status)
{
	FText StateMessage;

	switch (status) {
	case 0:
		StateMessage = FText::FromString("BHAPTICS_SETTINGS_SUCCESS");
		break;
	case 1:
		StateMessage = FText::FromString("NETWORK_ERROR");
		break;
	case 2:
		StateMessage = FText::FromString("API_KEY_INVALID");
		break;
	case 3:
		StateMessage = FText::FromString("APP_ID_INVALID");
		break;
	case 4:
		StateMessage = FText::FromString("APPLICATION_NOT_DEPLOY");
		break;
	case 10:
		StateMessage = FText::FromString("PARSING_FAILED");
		break;
	case 5:
		StateMessage = FText::FromString("NOT_CHANGED");		//Todo : compare deploy version
		break;
	case -1:
		StateMessage = FText::FromString("NEED BHAPTICS_SETTINGS VALIDATE");
		break;
	default:
		StateMessage = FText::FromString("UNKNOWN_ISSUES");
		break;
	}

	IDetailCategoryBuilder& CategoryBuilder = SavedLayoutBuilder->EditCategory("bHaptics Setting", FText::GetEmpty());
	TSharedRef<SHorizontalBox> Result = SNew(SHorizontalBox)
		.ToolTipText(FText::GetEmpty())
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
#if Bhaptics_UE_VERSION_EQUALS_OR_BELOW(5,0)
			SNew(SImage)
			.Image(FEditorStyle::GetBrush(FName("SettingsEditor.GoodIcon")))
#else
			SNew(SImage)
			.Image(FAppStyle::GetBrush(FName("SettingsEditor.GoodIcon")))
#endif
		]

	// Notice
	+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(16.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::Red)
			.ShadowColorAndOpacity(FLinearColor::Black)
			.ShadowOffset(FVector2D::UnitVector)
			.Text(StateMessage)
		]
	+ SHorizontalBox::Slot().HAlign(HAlign_Right)
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor::Green)
			.Text(LOCTEXT("Link bHaptics", "Link bHaptics"))
			.HAlign(HAlign_Center).VAlign(VAlign_Center)
			.OnClicked_Raw(this, &FBhapticsDetailsCustomization::LinkSetting)
		]
	;

	CategoryBuilder.AddCustomRow(LOCTEXT("", ""), false).WholeRowWidget[
		Result
	];
}


#undef LOCTEXT_NAMESPACE