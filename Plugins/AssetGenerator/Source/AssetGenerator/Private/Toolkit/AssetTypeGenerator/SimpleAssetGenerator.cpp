#include "Toolkit/AssetTypeGenerator/SimpleAssetGenerator.h"
#include "UE427AssetPath.h"
#include "Dom/JsonObject.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Particles/ParticleSystem.h"
#include "Engine/TextureRenderTarget2D.h"


#include "Engine/CompositeDataTable.h"
#include "UObject/MetaData.h"

static TMap<FRigidBodyIndexPair, bool> ReadCollisionDisableTable(const TSharedPtr<FJsonObject>& AssetData, const UPhysicsAsset* PhysicsAsset) {
	TMap<FRigidBodyIndexPair, bool> Result;
	for (const TSharedPtr<FJsonValue>& Value : AssetData->GetArrayField(TEXT("CollisionDisableTable"))) {
		const TSharedPtr<FJsonObject> Row = Value->AsObject();
		const int32 BodyA = Row->GetIntegerField(TEXT("BodyA"));
		const int32 BodyB = Row->GetIntegerField(TEXT("BodyB"));
		checkf(PhysicsAsset->SkeletalBodySetups.IsValidIndex(BodyA) && PhysicsAsset->SkeletalBodySetups.IsValidIndex(BodyB) && BodyA != BodyB, TEXT("Invalid physics collision pair: %d, %d"), BodyA, BodyB);
		Result.Add(FRigidBodyIndexPair(BodyA, BodyB), Row->GetBoolField(TEXT("Disabled")));
	}
	return Result;
}

void USimpleAssetGenerator::CreateAssetPackage() {
	ParticlePlaceholderReason = GetParticlePlaceholderReason();
	UPackage* NewPackage = CreatePackage(*GetPackageName().ToString());
	UClass* AssetClass = GetAssetObjectClass();
	UObject* NewAssetObject = NewObject<UObject>(NewPackage, AssetClass, GetAssetName(), RF_Public | RF_Standalone);
	SetPackageAndAsset(NewPackage, NewAssetObject);
	
	PopulateSimpleAssetWithData(NewAssetObject);
}

void USimpleAssetGenerator::OnExistingPackageLoaded() {
	ParticlePlaceholderReason = GetParticlePlaceholderReason();
	UObject* ExistingAssetObject = GetAsset<UObject>();
	if (!UE427IsEmpty(ParticlePlaceholderReason)) {
		const auto System = CastChecked<UParticleSystem>(ExistingAssetObject);
		if (!UE427IsEmpty(System->Emitters)) {
			UE_LOG(LogAssetGenerator, Warning, TEXT("Keeping existing particle system %s; its cooked dump requires distribution reconstruction."), *System->GetPathName());
			return;
		}
	}

	if (!IsSimpleAssetUpToDate(ExistingAssetObject)) {
		UE_LOG(LogAssetGenerator, Display, TEXT("%s %s is not up to date, regenerating data"), *ExistingAssetObject->GetClass()->GetName(), *ExistingAssetObject->GetPathName());
		
		PopulateSimpleAssetWithData(ExistingAssetObject);
	}
}

FString USimpleAssetGenerator::GetParticlePlaceholderReason() const {
	if (!GetAssetObjectClass()->IsChildOf(UParticleSystem::StaticClass())) return FString();
	FString MissingDistribution;
	TFunction<void(const TSharedPtr<FJsonValue>&, const FString&)> Visit;
	Visit = [&](const TSharedPtr<FJsonValue>& Value, const FString& Path) {
		if (!Value || !UE427IsEmpty(MissingDistribution)) return;
		if (Value->Type == EJson::Object) {
			const auto Object = Value->AsObject();
			int32 Distribution;
			if (Object->HasField(TEXT("Table")) && Object->TryGetNumberField(TEXT("Distribution"), Distribution) && Distribution == INDEX_NONE) {
				MissingDistribution = Path;
				return;
			}
			for (const auto& Pair : Object->Values) Visit(Pair.Value, Path + TEXT(".") + Pair.Key);
		}
		else if (Value->Type == EJson::Array) {
			const auto& Array = Value->AsArray();
			for (int32 Index = 0; Index < Array.Num(); ++Index) Visit(Array[Index], FString::Printf(TEXT("%s[%d]"), *Path, Index));
		}
	};
	Visit(MakeShared<FJsonValueObject>(GetAssetData()->GetObjectField(TEXT("AssetObjectData"))), GetAssetName().ToString());
	for (const auto& Entry : GetObjectSerializer()->FinalizeSerialization()) {
		FString Name;
		Entry->AsObject()->TryGetStringField(TEXT("ObjectName"), Name);
		Visit(Entry, Name);
	}
	return MissingDistribution;
}

void USimpleAssetGenerator::PopulateSimpleAssetWithData(UObject* Asset) {
	if (!UE427IsEmpty(ParticlePlaceholderReason)) {
		auto ParticleSystem = CastChecked<UParticleSystem>(Asset);
		check(UE427IsEmpty(ParticleSystem->Emitters));
		auto Metadata = Asset->GetOutermost()->GetMetaData();
		Metadata->SetValue(Asset, TEXT("CASPlaceholder"), TEXT("CookedParticleMissingDistributions"));
		Metadata->SetValue(Asset, TEXT("CASPlaceholderReason"), *ParticlePlaceholderReason);
		Asset->MarkPackageDirty();
		MarkAssetChanged();
		UE_LOG(LogAssetGenerator, Warning, TEXT("Created empty particle placeholder %s: cooked editor distribution missing at %s. This placeholder produces no visual effect."), *Asset->GetPathName(), *ParticlePlaceholderReason);
		return;
	}
	const TSharedPtr<FJsonObject> AssetData = GetAssetData();
	const TSharedPtr<FJsonObject> AssetObjectProperties = AssetData->GetObjectField(TEXT("AssetObjectData"));

	GetObjectSerializer()->DeserializeObjectProperties(AssetObjectProperties.ToSharedRef(), Asset);
	if (UCompositeDataTable* Table = Cast<UCompositeDataTable>(Asset)) Table->AppendParentTables({});


	if (UTextureRenderTarget2D* RenderTarget = Cast<UTextureRenderTarget2D>(Asset)) {
		RenderTarget->UpdateResource();
	}
	if (UParticleSystem* ParticleSystem = Cast<UParticleSystem>(Asset)) {
		ParticleSystem->UpdateAllModuleLists();
		ParticleSystem->BuildEmitters();
	}
	if (UPhysicsAsset* PhysicsAsset = Cast<UPhysicsAsset>(Asset); PhysicsAsset && AssetData->HasField(TEXT("CollisionDisableTable"))) {
		PhysicsAsset->CollisionDisableTable = ReadCollisionDisableTable(AssetData, PhysicsAsset);
		PhysicsAsset->UpdateBodySetupIndexMap();
		PhysicsAsset->UpdateBoundsBodiesArray();
	}
}

bool USimpleAssetGenerator::IsSimpleAssetUpToDate(UObject* Asset) const {
	if (!UE427IsEmpty(ParticlePlaceholderReason)) {
		const auto System = CastChecked<UParticleSystem>(Asset);
		return UE427IsEmpty(System->Emitters) && FString(Asset->GetOutermost()->GetMetaData()->GetValue(Asset, TEXT("CASPlaceholder"))) == TEXT("CookedParticleMissingDistributions");
	}
	const TSharedPtr<FJsonObject> AssetData = GetAssetData();
	const TSharedPtr<FJsonObject> AssetObjectProperties = AssetData->GetObjectField(TEXT("AssetObjectData"));

	if (!GetObjectSerializer()->AreObjectPropertiesUpToDate(AssetObjectProperties.ToSharedRef(), Asset)) return false;
	if (const UPhysicsAsset* PhysicsAsset = Cast<UPhysicsAsset>(Asset); PhysicsAsset && AssetData->HasField(TEXT("CollisionDisableTable"))) {
		const auto Expected = ReadCollisionDisableTable(AssetData, PhysicsAsset);
		if (Expected.Num() != PhysicsAsset->CollisionDisableTable.Num()) return false;
		for (const auto& Pair : Expected) {
			const bool* Existing = PhysicsAsset->CollisionDisableTable.Find(Pair.Key);
			if (!Existing || *Existing != Pair.Value) return false;
		}
	}
	return true;
}

void USimpleAssetGenerator::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
	if (!UE427IsEmpty(GetParticlePlaceholderReason())) return;
	if (GetCurrentStage() == EAssetGenerationStage::CONSTRUCTION) {
		PopulateReferencedObjectsDependencies(OutDependencies);
	}
}
