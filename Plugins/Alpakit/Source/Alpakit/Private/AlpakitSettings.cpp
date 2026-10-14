#include "AlpakitSettings.h"

const TCHAR* LexToString(EAlpakitStartGameType StartGameType) {	
	switch (StartGameType) {
	case EAlpakitStartGameType::STEAM:
		return TEXT("Steam");
	case EAlpakitStartGameType::CUSTOM:
		return TEXT("Custom");
	default:
		return TEXT("");
	}
}

UAlpakitSettings* UAlpakitSettings::Get()
{
    return GetMutableDefault<UAlpakitSettings>();
}

void UAlpakitSettings::SaveSettings()
{
    SaveConfig();
}

TArray<FString> UAlpakitSettings::GetAllowedBuildConfigurations() const
{
	return TArray<FString>
	{
		TEXT(""),
		LexToString(EBuildConfiguration::Debug      ),
		LexToString(EBuildConfiguration::DebugGame  ),
		LexToString(EBuildConfiguration::Development),
		LexToString(EBuildConfiguration::Test       ),
		LexToString(EBuildConfiguration::Shipping   ),
	};
}

EBuildConfiguration UAlpakitSettings::GetBuildConfiguration() const
{
	EBuildConfiguration ResultBuildConfiguration = EBuildConfiguration::Shipping;
	if (!OverrideBuildConfiguration.IsEmpty())
	{
		if (!LexTryParseString(ResultBuildConfiguration, *OverrideBuildConfiguration))
		{
			ResultBuildConfiguration = EBuildConfiguration::Shipping;
		}
	}
	return ResultBuildConfiguration;
}

const TMap<FString, FAlpakitTargetSettings> UAlpakitSettings::GetPlatformTargetSettings() const {
	return {
		        {TEXT("WindowsNoEditor"), WindowsGameTargetSettings},


			};
}
