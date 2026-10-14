#include "Toolkit/AssetTypeGenerator/DataAssetGenerator.h"
#include "UE427AssetPath.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundSubmix.h"
#include "Sound/SoundConcurrency.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Particles/ParticleSystem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"


#include "Sound/SoundSourceBus.h"
#include "Sound/AudioBus.h"
#include "Engine/CompositeDataTable.h"

#include "SubmixEffects/AudioMixerSubmixEffectEQ.h"
#include "SubmixEffects/AudioMixerSubmixEffectReverb.h"
#include "SubmixEffects/AudioMixerSubmixEffectDynamicsProcessor.h"
#include "SubmixEffects/SubmixEffectMultiBandCompressor.h"
#include "SourceEffects/SourceEffectEQ.h"
#include "SourceEffects/SourceEffectWaveShaper.h"
#include "SourceEffects/SourceEffectStereoDelay.h"
#include "Sound/SoundEffectSource.h"
#include "Materials/MaterialFunctionMaterialLayer.h"
#include "Materials/MaterialFunctionInstance.h"
#include "LandscapeLayerInfoObject.h"

UClass* UDataAssetGenerator::GetAssetObjectClass() const {
	return FindObjectChecked<UClass>(NULL, *GetAssetClassPath().ToString());
}

bool UDataAssetGenerator::DefersAudioProperties() const {
	return !GetAssetClassPath().GetPackageName().ToString().StartsWith(TEXT("/Script/")) || GetAssetClassPath() == FTopLevelAssetPath(USoundClass::StaticClass()) || GetAssetClassPath() == FTopLevelAssetPath(USoundSubmix::StaticClass()) || GetAssetClassPath() == FTopLevelAssetPath(USoundfieldSubmix::StaticClass());
}

void UDataAssetGenerator::CreateAssetPackage() {
	if (!DefersAudioProperties()) { Super::CreateAssetPackage(); return; }
	UPackage* Package = CreatePackage(*GetPackageName().ToString());
	SetPackageAndAsset(Package, NewObject<UObject>(Package, GetAssetObjectClass(), GetAssetName(), RF_Public | RF_Standalone));
}

void UDataAssetGenerator::OnExistingPackageLoaded() {
	if (!DefersAudioProperties()) Super::OnExistingPackageLoaded();
}

void UDataAssetGenerator::PopulateAssetWithData() {
	if (!DefersAudioProperties()) { Super::PopulateAssetWithData(); return; }
	UObject* Asset = GetAsset<UObject>();
	if (!IsSimpleAssetUpToDate(Asset)) {
		PopulateSimpleAssetWithData(Asset);
		MarkAssetChanged();
	}
}

void UDataAssetGenerator::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
    if (!GetAssetClassPath().GetPackageName().ToString().StartsWith(TEXT("/Script/"))) {
        if (GetCurrentStage() == EAssetGenerationStage::CONSTRUCTION) {
            OutDependencies.Add(FPackageDependency{GetAssetClassPath().GetPackageName(), EAssetGenerationStage::DATA_POPULATION});
        }
        if (GetCurrentStage() == EAssetGenerationStage::DATA_POPULATION) {
            OutDependencies.Add(FPackageDependency{GetAssetClassPath().GetPackageName(), EAssetGenerationStage::CDO_FINALIZATION});
            TArray<FPackageDependency> Dependencies;
            PopulateReferencedObjectsDependencies(Dependencies);
            for (const FPackageDependency& Dependency : Dependencies) OutDependencies.Add(FPackageDependency{Dependency.PackageName, EAssetGenerationStage::CONSTRUCTION});
        }
        return;
    }
	if (!DefersAudioProperties()) { Super::PopulateStageDependencies(OutDependencies); return; }
	if (GetCurrentStage() == EAssetGenerationStage::DATA_POPULATION) {
		TArray<FPackageDependency> Dependencies;
		PopulateReferencedObjectsDependencies(Dependencies);
		for (const FPackageDependency& Dependency : Dependencies) OutDependencies.Add(FPackageDependency{Dependency.PackageName, EAssetGenerationStage::CONSTRUCTION});
	}
}

FTopLevelAssetPath UDataAssetGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UDataAsset::StaticClass());
}

void UDataAssetGenerator::GetAdditionallyHandledAssetClasses(TArray<FTopLevelAssetPath>& OutExtraAssetClasses) {
    for (const TCHAR* Path : { TEXT("/Script/Synthesis.SourceEffectConvolutionReverbPreset"), TEXT("/Script/Synthesis.AudioImpulseResponse"), TEXT("/Script/HTN.HTN") }) {
        if (UClass* Class = LoadObject<UClass>(nullptr, Path, nullptr, LOAD_Quiet | LOAD_NoWarn)) OutExtraAssetClasses.Add(FTopLevelAssetPath(Class));
    }
    OutExtraAssetClasses.Add(FTopLevelAssetPath(UMaterialFunctionMaterialLayerInstance::StaticClass()));
    OutExtraAssetClasses.Add(FTopLevelAssetPath(UMaterialFunctionInstance::StaticClass()));
    OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundfieldSubmix::StaticClass()));
    OutExtraAssetClasses.Add(FTopLevelAssetPath(ULandscapeLayerInfoObject::StaticClass()));
    if (UClass* Preset = LoadObject<UClass>(nullptr, TEXT("/Script/ResonanceAudio.ResonanceAudioReverbPluginPreset"), nullptr, LOAD_Quiet | LOAD_NoWarn)) {
        OutExtraAssetClasses.Add(FTopLevelAssetPath(Preset));
    }
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundClass::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundMix::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundAttenuation::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundSubmix::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundConcurrency::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UPhysicsAsset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UParticleSystem::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UTextureRenderTarget2D::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UCanvasRenderTarget2D::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UHapticFeedbackEffect_Curve::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundSourceBus::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UAudioBus::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(UCompositeDataTable::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(TEXT("/Script/ResonanceAudio.ResonanceAudioSpatializationSourceSettings")));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USubmixEffectSubmixEQPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USubmixEffectReverbPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USubmixEffectDynamicsProcessorPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(TEXT("/Script/Synthesis.SubmixEffectMultibandCompressorPreset")));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USourceEffectEQPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USourceEffectWaveShaperPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USourceEffectStereoDelayPreset::StaticClass()));
	OutExtraAssetClasses.Add(FTopLevelAssetPath(USoundEffectSourcePresetChain::StaticClass()));
	// Handle any UDataAsset-derived classes
	TArray<UClass*> DerivedClasses;
	GetDerivedClasses(UDataAsset::StaticClass(), DerivedClasses, true);
	for (UClass* DerivedClass : DerivedClasses) {
		OutExtraAssetClasses.Add(FTopLevelAssetPath(DerivedClass));
	}

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FARFilter Filter;
	Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	for (const UClass* DerivedClass : DerivedClasses) {
		Filter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, FObjectPropertyBase::GetExportPath(DerivedClass, nullptr, nullptr, PPF_None));
	}
	Filter.bRecursiveClasses = true;
	TArray<FAssetData> BlueprintDataAssets;
	AssetRegistry.GetAssets(Filter, BlueprintDataAssets);
	for (const FAssetData& BlueprintDataAsset : BlueprintDataAssets) {
		FString GeneratedClassPath;
		BlueprintDataAsset.GetTagValue(FBlueprintTags::GeneratedClassPath, GeneratedClassPath);
		OutExtraAssetClasses.Add(FTopLevelAssetPath(GeneratedClassPath));
	}
}
