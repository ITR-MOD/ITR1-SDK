#include "Toolkit/AssetTypeGenerator/SoundWaveGenerator.h"
#include "UE427AssetPath.h"
#include "Dom/JsonObject.h"
#include "Factories/SoundFactory.h"
#include "Misc/FileHelper.h"
#include "Misc/SecureHash.h"
#include "Sound/SoundWave.h"
#include "UObject/MetaData.h"

#include "Interfaces/IAudioFormat.h"
#include "Serialization/MemoryWriter.h"

UClass* USoundWaveGenerator::GetAssetObjectClass() const { return USoundWave::StaticClass(); }
FTopLevelAssetPath USoundWaveGenerator::GetAssetClass() { return FTopLevelAssetPath(USoundWave::StaticClass()); }
void USoundWaveGenerator::CreateAssetPackage() { ImportWave(); }

void USoundWaveGenerator::OnExistingPackageLoaded() {
	USoundWave* Sound = GetAsset<USoundWave>();
	const FString ExpectedHash = GetAssetData()->GetStringField(TEXT("SourceWaveHash"));
	if (Sound->GetOutermost()->GetMetaData()->GetValue(Sound, TEXT("CASSourceWaveHash")) != ExpectedHash || !IsSimpleAssetUpToDate(Sound)) ImportWave();
}

void USoundWaveGenerator::ImportWave() {
	TArray<uint8> Bytes;
	FString Format = TEXT("WAV");
	GetAssetData()->TryGetStringField(TEXT("SourceWaveFormat"), Format);
	if (Format != TEXT("WAV") && Format != TEXT("BINKA")) {
		FailGeneration(FString::Printf(TEXT("Unsupported SoundWave source format: %s"), *Format));
		return;
	}
	if (!FFileHelper::LoadFileToArray(Bytes, *GetAdditionalDumpFilePath(TEXT(""), Format == TEXT("BINKA") ? TEXT("binka") : TEXT("wav"))) || Bytes.Num() < 44) {
		FailGeneration(TEXT("SoundWave WAV payload is missing or truncated."));
		return;
	}
	const FString ExpectedHash = GetAssetData()->GetStringField(TEXT("SourceWaveHash"));
	if (FMD5::HashBytes(Bytes.GetData(), Bytes.Num()) != ExpectedHash) {
		FailGeneration(TEXT("SoundWave WAV payload does not match the CAS source hash."));
		return;
	}
	if (Format == TEXT("BINKA")) { FailGeneration(TEXT("Re-export audio as WAV for UE4.27.")); return; }
	UPackage* Package = CreatePackage(*GetPackageName().ToString());
	USoundFactory* SoundFactory = NewObject<USoundFactory>();
	SoundFactory->bAutoCreateCue = false;
	SoundFactory->SuppressImportDialogs();
	const uint8* Buffer = Bytes.GetData();
	USoundWave* Sound = Cast<USoundWave>(SoundFactory->FactoryCreateBinary(USoundWave::StaticClass(), Package, GetAssetName(), RF_Public | RF_Standalone, nullptr, TEXT("wav"), Buffer, Buffer + Bytes.Num(), GWarn));
	SoundFactory->CleanUp();
	if (!Sound) {
		FailGeneration(TEXT("Unreal could not import the SoundWave WAV payload."));
		return;
	}
	if (!GetAssetPackage()) SetPackageAndAsset(Package, Sound);
	PopulateSimpleAssetWithData(Sound);
	Package->GetMetaData()->SetValue(Sound, TEXT("CASSourceWaveHash"), *ExpectedHash);
	MarkAssetChanged();
}
