#include "AssetGeneration/AssetGeneratorLocalSettings.h"
#include "UE427AssetPath.h"
#include "Toolkit/AssetGeneration/AssetTypeGenerator.h"
#include "FileHelpers.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Toolkit/PropertySerializer.h"
#include "Toolkit/AssetGeneration/AssetGenerationUtil.h"
#include "Toolkit/AssetTypeGenerator/DataAssetGenerator.h"
#include "Engine/DataAsset.h"
#include "UObject/MetaData.h"
#include "UObject/CoreRedirects.h"
#include "Components/ActorComponent.h"
#include "UObject/LinkerLoad.h"
#include "UObject/PackageFileSummary.h"
#include "HAL/FileManager.h"
#include "ISourceControlModule.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY(LogAssetGenerator)

bool UAssetTypeGenerator::SaveGeneratedPackages(const TArray<UPackage*>& Packages, bool bOnlyDirty) {
	if (ISourceControlModule::Get().IsEnabled()) return UEditorLoadingAndSavingUtils::SavePackages(Packages, bOnlyDirty);
	bool bSaved = true;
	for (UPackage* Package : Packages) {
		if (!Package || (bOnlyDirty && !Package->IsDirty())) continue;
		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), Package->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		UE_LOG(LogAssetGenerator, Display, TEXT("Saving generated package %s"), *Package->GetName());
        bSaved &= UPackage::SavePackage(Package, nullptr, SaveArgs.TopLevelFlags, *Filename, GError, nullptr, false, true, SaveArgs.SaveFlags);
	}
	return bSaved;
}

FString UAssetTypeGenerator::GetAdditionalDumpFilePath(const FString& Postfix, const FString& Extension) const {
	FString Filename = FPackageName::GetShortName(GetPackageName());
		
	if (Postfix.Len() > 0) {
		Filename.AppendChar('-').Append(Postfix);
	}
	if (Extension.Len() > 0) {
		if (Extension[0] != '.')
			Filename.AppendChar('.');
		Filename.Append(Extension);
	}
	return FPaths::Combine(PackageBaseDirectory, Filename);
}

void UAssetTypeGenerator::PopulateReferencedObjectsDependencies(TArray<FPackageDependency>& OutDependencies) const {
	const TSharedPtr<FJsonObject> AssetData = GetAssetData();
	const TSharedPtr<FJsonObject> AssetObjectProperties = AssetData->GetObjectField(TEXT("AssetObjectData"));
	const TArray<TSharedPtr<FJsonValue>> ReferencedObjects = AssetObjectProperties->GetArrayField(TEXT("$ReferencedObjects"));
		
	TArray<FString> OutReferencedPackages;
	GetObjectSerializer()->CollectReferencedPackages(ReferencedObjects, OutReferencedPackages);

	for (const FString& DependencyPackageName : OutReferencedPackages) {
		OutDependencies.Add(FPackageDependency{*DependencyPackageName, EAssetGenerationStage::CDO_FINALIZATION});
	}
}

void UAssetTypeGenerator::MoveToTransientPackageAndRename(UObject* Object) {
	Object->Rename(NULL, GetTransientPackage(), REN_DoNothing);
	Object->SetFlags(RF_Transient);
}

UAssetTypeGenerator::UAssetTypeGenerator() {
	this->PropertySerializer = CreateDefaultSubobject<UPropertySerializer>(TEXT("PropertySerializer"));
	this->ObjectSerializer = CreateDefaultSubobject<UObjectHierarchySerializer>(TEXT("ObjectSerializer"));
	this->ObjectSerializer->SetPropertySerializer(PropertySerializer);

	this->CurrentStage = EAssetGenerationStage::CONSTRUCTION;
	this->AssetPackage = NULL;
	this->AssetObject = NULL;
	this->bUsingExistingPackage = false;
	this->bAssetChanged = false;
	this->bHasAssetEverBeenChanged = false;
	this->bIsGeneratingPublicProject = false;
}

void UAssetTypeGenerator::InitializeInternal(const FString& DumpRootDirectory, const FString& InPackageBaseDirectory, const FName InPackageName, const TSharedPtr<FJsonObject> RootFileObject, bool bGeneratePublicProject) {
	this->DumpRootDirectory = DumpRootDirectory;
	this->PackageBaseDirectory = InPackageBaseDirectory;
	this->PackageName = FName(*RootFileObject->GetStringField(TEXT("AssetPackage")));
	this->AssetName = FName(*RootFileObject->GetStringField(TEXT("AssetName")));
	this->AssetClassPath = ResolveDumpAssetClass(RootFileObject);
	checkf(this->PackageName == InPackageName, TEXT("InitializeInternal called with inconsistent package name. Externally provided name was '%s', but internal dump package name is '%s'"),
		*InPackageName.ToString(), *this->PackageName.ToString());

	const TArray<TSharedPtr<FJsonValue>> ObjectHierarchy = RootFileObject->GetArrayField(TEXT("ObjectHierarchy"));
	for (const auto& Entry : ObjectHierarchy) {
		const auto Object = Entry->AsObject();
		DependencyObjects.Add(Object->GetIntegerField(TEXT("ObjectIndex")), Object);
	}
	this->ObjectSerializer->InitializeForDeserialization(ObjectHierarchy);
	this->AssetData = RootFileObject->GetObjectField(TEXT("AssetSerializedData"));
	this->bIsGeneratingPublicProject = bGeneratePublicProject;
	PostInitializeAssetGenerator();
}

bool UAssetTypeGenerator::ValidateDependencyObjects(const TSet<FName>& RequiredPackages, FString& OutFailure) const {
	TSet<int32> ReferencedImports;
	const TSet<FString> IndexFields = {TEXT("Outer"), TEXT("ObjectClass"), TEXT("SuperStruct"), TEXT("ClassWithin"), TEXT("ClassDefaultObject"), TEXT("Class"), TEXT("PropertyClass"), TEXT("InterfaceClass"), TEXT("MetaClass"), TEXT("Struct"), TEXT("Enum"), TEXT("SignatureFunction"), TEXT("WidgetTree"), TEXT("SimpleConstructionScript")};
	TFunction<void(const TSharedPtr<FJsonValue>&, bool)> CollectReferences;
	CollectReferences = [&](const TSharedPtr<FJsonValue>& Value, bool bIndex) {
		if (!Value.IsValid()) return;
		if (Value->Type == EJson::Number && bIndex) ReferencedImports.Add(static_cast<int32>(Value->AsNumber()));
		else if (Value->Type == EJson::Array) {
			for (const auto& Child : Value->AsArray()) CollectReferences(Child, bIndex);
		}
		else if (Value->Type == EJson::Object) {
			for (const auto& Field : Value->AsObject()->Values)
				CollectReferences(Field.Value, IndexFields.Contains(Field.Key) || Field.Key == TEXT("$ReferencedObjects") || Field.Key == TEXT("Animations") || Field.Key == TEXT("ComponentTemplates"));
		}
	};
	CollectReferences(MakeShared<FJsonValueObject>(GetAssetData()), false);
	for (const auto& Entry : DependencyObjects)
		if (Entry.Value->GetStringField(TEXT("Type")) == TEXT("Export")) CollectReferences(MakeShared<FJsonValueObject>(Entry.Value), false);
	TArray<int32> Pending = ReferencedImports.Array();
	for (int32 Position = 0; Position < Pending.Num(); ++Position) {
		const auto* Object = DependencyObjects.Find(Pending[Position]);
		int32 OuterIndex = INDEX_NONE;
		if (Object && (*Object)->TryGetNumberField(TEXT("Outer"), OuterIndex) && !ReferencedImports.Contains(OuterIndex)) {
			ReferencedImports.Add(OuterIndex);
			Pending.Add(OuterIndex);
		}
	}
	for (const auto& Entry : DependencyObjects) {
		if (!ReferencedImports.Contains(Entry.Key)) continue;
		if (Entry.Value->GetStringField(TEXT("Type")) != TEXT("Import")) continue;
		TArray<TSharedPtr<FJsonObject>> Chain;
		TSet<int32> Visited;
		int32 Index = Entry.Key;
		FName RootPackage;
		while (Index != INDEX_NONE) {
			const auto* Object = DependencyObjects.Find(Index);
			if (!Object || Visited.Contains(Index)) {
				OutFailure = FString::Printf(TEXT("Invalid or cyclic import outer reference at object index %d."), Index);
				return false;
			}
			Visited.Add(Index);
			if ((*Object)->GetStringField(TEXT("Type")) != TEXT("Import")) break;
			Chain.Add(*Object);
			if (!(*Object)->HasField(TEXT("Outer"))) {
				RootPackage = FName(*(*Object)->GetStringField(TEXT("ObjectName")));
				break;
			}
			Index = (*Object)->GetIntegerField(TEXT("Outer"));
		}
		if (!RequiredPackages.Contains(RootPackage) || RootPackage == PackageName) continue;
		UObject* Outer = FindPackage(nullptr, *RootPackage.ToString());
		if (!Outer) Outer = LoadPackage(nullptr, *RootPackage.ToString(), LOAD_Quiet | LOAD_NoWarn);
		if (!Outer) {
			OutFailure = FString::Printf(TEXT("Dependency package %s exists but could not be loaded."), *RootPackage.ToString());
			return false;
		}
		for (int32 Part = Chain.Num() - 2; Part >= 0; --Part) {
			if (Part < Chain.Num() - 2 && GetCurrentStage() <= EAssetGenerationStage::DATA_POPULATION) break;
			const FString ClassPackageName = Chain[Part]->GetStringField(TEXT("ClassPackage"));
			UPackage* ClassPackage = FindPackage(nullptr, *ClassPackageName);
			if (!ClassPackage) ClassPackage = LoadPackage(nullptr, *ClassPackageName, LOAD_Quiet | LOAD_NoWarn);
			const FName ClassName(*Chain[Part]->GetStringField(TEXT("ClassName")));
			UClass* ObjectClass = ClassPackage ? FindObjectFast<UClass>(ClassPackage, ClassName) : nullptr;
			if (!ObjectClass) ObjectClass = LoadObject<UClass>(nullptr, *FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(ClassPackageName + TEXT(".") + ClassName.ToString())).ToString(), nullptr, LOAD_Quiet | LOAD_NoWarn);
			const FName ObjectName(*Chain[Part]->GetStringField(TEXT("ObjectName")));
			UObject* Object = ObjectClass ? StaticFindObjectFast(ObjectClass, Outer, ObjectName) : nullptr;
			if (!Object && ObjectClass == UClass::StaticClass()) Object = LoadObject<UClass>(nullptr, *FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(Outer->GetPathName() + TEXT(".") + ObjectName.ToString())).ToString(), nullptr, LOAD_Quiet | LOAD_NoWarn);
			if (!Object && ObjectClass == UClass::StaticClass() && Outer->GetPathName().StartsWith(TEXT("/Script/"))) {
				Object = FindObject<UScriptStruct>(Outer, *ObjectName.ToString());
				if (!Object) Object = FindObject<UEnum>(Outer, *ObjectName.ToString());
			}
			if (!Object && ObjectClass == UFunction::StaticClass()) {
				if (UClass* OwnerClass = Cast<UClass>(Outer)) Object = OwnerClass->FindFunctionByName(ObjectName);
			}
			if (!Object && ObjectClass && ObjectClass->IsChildOf(UActorComponent::StaticClass()) && ObjectName.ToString().EndsWith(TEXT("_GEN_VARIABLE"))) {
				if (UClass* OwnerClass = Cast<UClass>(Outer)) Object = OwnerClass->FindArchetype(ObjectClass, FName(*ObjectName.ToString().LeftChop(13)));
				if (Object && !Object->IsA(ObjectClass)) Object = nullptr;
			}
			if (!Object) {
				TArray<UObject*> PackageObjects;
				GetObjectsWithPackage(Outer->GetOutermost(), PackageObjects, false);
				for (UObject* Candidate : PackageObjects)
					if (ObjectClass && Candidate->IsA(ObjectClass)) UE_LOG(LogAssetGenerator, Verbose, TEXT("Dependency candidate: %s"), *Candidate->GetPathName());
				OutFailure = FString::Printf(TEXT("Dependency object %s.%s (class %s.%s) is unavailable after dependency generation."), *Outer->GetPathName(), *ObjectName.ToString(), *ClassPackageName, *ClassName.ToString());
				return false;
			}
			Outer = Object;
		}
	}
	return true;
}

void UAssetTypeGenerator::ConstructAssetAndPackage() {
	UPackage* ExistingPackage = FindPackage(NULL, *PackageName.ToString());
	bool bLoadableFile = false;
	FString ExistingFilename;
	if (FPackageName::DoesPackageExist(PackageName.ToString(), nullptr, &ExistingFilename)) {
		TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader(*ExistingFilename));
		FPackageFileSummary Summary;
		if (Reader) *Reader << Summary;
		bLoadableFile = Reader && !Reader->IsError() && !Summary.bUnversioned;
	}
	if (!ExistingPackage && bLoadableFile) {
		ExistingPackage = LoadPackage(NULL, *PackageName.ToString(), LOAD_Quiet);
	}
		
	if (ExistingPackage && bLoadableFile) ExistingPackage->FullyLoad();
	if (ExistingPackage && !bLoadableFile) ResetLoaders(ExistingPackage);
	UObject* ExistingAsset = ExistingPackage ? FindObject<UObject>(ExistingPackage, *AssetName.ToString()) : nullptr;
	if (!ExistingAsset) {
		//Make new package if we don't have existing one, make sure asset object is also allocated
		CreateAssetPackage();
		if (!UE427IsEmpty(GenerationFailure)) return;
		if (!AssetPackage || !AssetObject) {
			GenerationFailure = TEXT("Asset construction did not produce a package and asset object.");
			return;
		}
		AssetPackage->MarkAsFullyLoaded();

		//Make sure to mark package as changed because it has never been saved to disk before
		MarkAssetChanged();
	} else {
		//Package already exist, reuse it while making sure out asset is contained within
		UObject* AssetObject = ExistingAsset;
		if (AssetObject == NULL) {
			GenerationFailure = FString::Printf(TEXT("Existing package %s does not contain required asset %s."), *PackageName.ToString(), *AssetName.ToString());
			return;
		}

		//We need to verify package exists and provide meaningful error message, so user knows what is wrong
		checkf(AssetObject, TEXT("Existing package %s does not contain an asset named %s, requested by asset dump"), *PackageName.ToString(), *AssetName.ToString());
		SetPackageAndAsset(ExistingPackage, AssetObject);
			
		//Notify generator we are reusing existing package, so it can do additional cleanup and settings
		this->bUsingExistingPackage = true;
		OnExistingPackageLoaded();
	}
}

void UAssetTypeGenerator::MarkAssetChanged() {
	this->bAssetChanged = true;
	this->bHasAssetEverBeenChanged = true;
	if (this->AssetPackage) {
		this->AssetPackage->MarkPackageDirty();
	}
}

void UAssetTypeGenerator::SetPackageAndAsset(UPackage* NewPackage, UObject* NewAsset, bool bSetObjectMark) {
	checkf(CurrentStage == EAssetGenerationStage::CONSTRUCTION, TEXT("SetPackageAndAsset can only be called during CONSTRUCTION"));
	checkf(AssetPackage == NULL, TEXT("SetPackageAndAsset can only be called once during CreateAssetPackage"));
	
	check(NewPackage->GetFName() == GetPackageName());
	check(NewAsset->GetFName() == GetAssetName());
	check(NewAsset->GetOuter() == NewPackage);
	
	this->AssetPackage = NewPackage;
	this->AssetObject = NewAsset;

	ObjectSerializer->SetPackageForDeserialization(AssetPackage);
	if (bSetObjectMark) {
		ObjectSerializer->SetObjectMark(AssetObject, TEXT("$AssetObject$"));
	}
}

void UAssetTypeGenerator::PopulateAssetWithData() {
	this->bIsStageNotOverriden = true;
}

void UAssetTypeGenerator::FinalizeAssetCDO() {
	this->bIsStageNotOverriden = true;
}

void UAssetTypeGenerator::PreFinishAssetGeneration() {
	this->bIsStageNotOverriden = true;
}

FGeneratorStateAdvanceResult UAssetTypeGenerator::AdvanceGenerationState() {
	this->bIsStageNotOverriden = false;
	this->bAssetChanged = false;

	//Return early if we have already finished asset generation
	if (CurrentStage == EAssetGenerationStage::FINISHED) {
		return FGeneratorStateAdvanceResult{CurrentStage, false};
	}
	
	//Dispatch current stage call to the appropriate method
	if (CurrentStage == EAssetGenerationStage::CONSTRUCTION) {
		this->ConstructAssetAndPackage();
	}
	if (!UE427IsEmpty(GenerationFailure)) return FGeneratorStateAdvanceResult{CurrentStage, false};
	if (CurrentStage == EAssetGenerationStage::DATA_POPULATION) {
		this->PopulateAssetWithData();
	}
	if (CurrentStage == EAssetGenerationStage::CDO_FINALIZATION) {
		this->FinalizeAssetCDO();
	}
	if (CurrentStage == EAssetGenerationStage::PRE_FINSHED) {
		// TODO: Hackfix on top of previous hackfix, does the job though. Needs a better solution.
		if (AssetObject != NULL) this->PreFinishAssetGeneration();
	}
	if (!UE427IsEmpty(GenerationFailure)) return FGeneratorStateAdvanceResult{CurrentStage, false};
		
	//Increment current generation stage
	this->CurrentStage = (EAssetGenerationStage) ((int32) CurrentStage + 1);
	
    if (AssetObject && AssetPackage) {
        UMetaData* Metadata = AssetPackage->GetMetaData();
        if (CurrentStage == EAssetGenerationStage::FINISHED) {
            if (Metadata->HasValue(AssetObject, TEXT("CASGenerationIncomplete"))) {
                Metadata->RemoveValue(AssetObject, TEXT("CASGenerationIncomplete"));
                MarkAssetChanged();
            }
        } else if (bAssetChanged) {
            Metadata->SetValue(AssetObject, TEXT("CASGenerationIncomplete"), TEXT("true"));
        }
    }
	//Force package to be saved to disk if it has been marked as changed, which should have also marked it as dirty
	if (bAssetChanged) {
		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(AssetPackage);
		GetAdditionalPackagesToSave(PackagesToSave);
		if (!SaveGeneratedPackages(PackagesToSave)) {
			FailGeneration(FString::Printf(TEXT("Failed to save generated package %s"), *GetPackageName().ToString()));
			return FGeneratorStateAdvanceResult{CurrentStage, false};
		}
		
		this->bAssetChanged = false;
		this->bHasAssetEverBeenChanged = true;
	}
	return FGeneratorStateAdvanceResult{CurrentStage, bIsStageNotOverriden};
}

FString UAssetTypeGenerator::GetAssetFilePath(const FString& RootDirectory, FName PackageName) {
	const FString ShortPackageName = FPackageName::GetShortName(PackageName);
	FString PackagePath = FPackageName::GetLongPackagePath(PackageName.ToString());
	PackagePath.RemoveAt(0);

	const FString PackageBaseDirectory = FPaths::Combine(RootDirectory, PackagePath);
	
	const FString AssetDumpFilename = FPaths::SetExtension(ShortPackageName, TEXT("json"));
	return FPaths::Combine(PackageBaseDirectory, AssetDumpFilename);
}

UAssetTypeGenerator* UAssetTypeGenerator::InitializeFromFile(const FString& RootDirectory, const FName PackageName, bool bGeneratePublicProject) {
	const FString AssetDumpFilePath = GetAssetFilePath(RootDirectory, PackageName);
	const FString PackageBaseDirectory = FPaths::GetPath(AssetDumpFilePath);

	//Return early if dump file is not found for this asset
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*AssetDumpFilePath)) {
		return NULL;
	}

	FString DumpFileStringContents;
	if (!FFileHelper::LoadFileToString(DumpFileStringContents, *AssetDumpFilePath)) {
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to load asset dump file %s"), *AssetDumpFilePath);
		return NULL;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DumpFileStringContents);
	TSharedPtr<FJsonObject> RootFileObject;
	if (!FJsonSerializer::Deserialize(Reader, RootFileObject)) {
		UE_LOG(LogAssetGenerator, Error, TEXT("Failed to parse asset dump file %s: invalid json"), *AssetDumpFilePath);
		return NULL;
	}

	const FTopLevelAssetPath AssetClass = ResolveDumpAssetClass(RootFileObject);
    if (UAssetGeneratorLocalSettings::Get()->bMeshesAndTexturesOnly && !UAssetGeneratorLocalSettings::IsMeshTextureOrMaterial(AssetClass.GetAssetName())) {
        UE_LOG(LogAssetGenerator, Display, TEXT("Filtered by Meshes and Textures only: %s (%s)"), *PackageName.ToString(), *AssetClass.ToString());
        return nullptr;
    }
	UClass* AssetTypeGenerator = FindGeneratorForClass(AssetClass);
    FString DumpKind;
    RootFileObject->TryGetStringField(TEXT("AssetClass"), DumpKind);
    if (!AssetTypeGenerator && DumpKind == TEXT("SimpleAsset") && AssetClass.IsValid() && !AssetClass.GetPackageName().ToString().StartsWith(TEXT("/Script/"))) {
        AssetTypeGenerator = UDataAssetGenerator::StaticClass();
    }
	if (AssetTypeGenerator == NULL) {
		UE_LOG(LogAssetGenerator, Error, TEXT("Asset generator not found for asset class '%s', loaded from %s"), *AssetClass.ToString(), *AssetDumpFilePath);
		return NULL;
	}

	UAssetTypeGenerator* NewGenerator = NewObject<UAssetTypeGenerator>(GetTransientPackage(), AssetTypeGenerator);
	NewGenerator->InitializeInternal(RootDirectory, PackageBaseDirectory, PackageName, RootFileObject, bGeneratePublicProject);
	return NewGenerator;
}

//Static class used to lazily populate serializer registry
class FAssetTypeGeneratorRegistry {
public:
	//Native classes should never get unloaded, so we can get away with using TSubclassOf without weak pointer
	TMap<FTopLevelAssetPath, TWeakObjectPtr<UClass>> Generators;

	//Constructor that will automatically populate registry serializers
	FAssetTypeGeneratorRegistry();

	static const FAssetTypeGeneratorRegistry& Get() {
		static FAssetTypeGeneratorRegistry AssetTypeSerializerRegistry{};
		return AssetTypeSerializerRegistry;
	}
};

FAssetTypeGeneratorRegistry::FAssetTypeGeneratorRegistry() {
	TArray<UClass*> AssetGeneratorClasses;
	GetDerivedClasses(UAssetTypeGenerator::StaticClass(), AssetGeneratorClasses, true);

	//Iterate classes in memory to resolve serializers
	TArray<UAssetTypeGenerator*> OutFoundInitializers;
	for (UClass* Class : AssetGeneratorClasses) {
		//Skip classes that are marked as Abstract
		if (Class->HasAnyClassFlags(CLASS_Abstract)) {
			continue;
		}
		//Skip classes that are not marked as Native
		if (!Class->HasAnyClassFlags(CLASS_Native)) {
			continue;
		}
            
		//Check asset type in class default object
		UAssetTypeGenerator* ClassDefaultObject = CastChecked<UAssetTypeGenerator>(Class->GetDefaultObject());
		if (ClassDefaultObject->GetAssetClass().IsValid()) {
			OutFoundInitializers.Add(ClassDefaultObject);
		}
	}

	//First register additional asset classes, so primary ones will overwrite them later
	for (UAssetTypeGenerator* Generator : OutFoundInitializers) {
		TArray<FTopLevelAssetPath> OutExtraAssetClasses;
		Generator->GetAdditionallyHandledAssetClasses(OutExtraAssetClasses);
		for (const FTopLevelAssetPath& AssetClass : OutExtraAssetClasses) {
			this->Generators.Add(AssetClass, Generator->GetClass());
		}
	}

	//Now setup primary asset classes and add serializers into array
	for (UAssetTypeGenerator* Generator : OutFoundInitializers) {
		const FTopLevelAssetPath AssetClass = Generator->GetAssetClass();
		this->Generators.Add(AssetClass, Generator->GetClass());
	}
}

TSubclassOf<UAssetTypeGenerator> UAssetTypeGenerator::FindGeneratorForClass(const FTopLevelAssetPath AssetClass) {
	const TWeakObjectPtr<UClass>* Class = FAssetTypeGeneratorRegistry::Get().Generators.Find(AssetClass);
	if (Class && Class->IsValid()) return Class->Get();
	UClass* AssetObjectClass = FindObject<UClass>(nullptr, *AssetClass.ToString());
	if (!AssetObjectClass && AssetClass.IsValid()) AssetObjectClass = LoadObject<UClass>(nullptr, *AssetClass.ToString(), nullptr, LOAD_Quiet | LOAD_NoWarn);
	if (AssetObjectClass && AssetObjectClass->IsChildOf(UDataAsset::StaticClass())) return UDataAssetGenerator::StaticClass();
	return nullptr;
}

TArray<TSubclassOf<UAssetTypeGenerator>> UAssetTypeGenerator::GetAllGenerators() {
	TArray<TSubclassOf<UAssetTypeGenerator>> ResultArray;

	for (const TPair<FTopLevelAssetPath, TWeakObjectPtr<UClass>>& Pair : FAssetTypeGeneratorRegistry::Get().Generators) {
		UClass* ResultingValue = Pair.Value.Get();
		if (ResultingValue) {
			ResultArray.AddUnique(ResultingValue);
		}
	}
	return ResultArray;
}

FTopLevelAssetPath UAssetTypeGenerator::ResolveDumpAssetClass(const TSharedPtr<FJsonObject>& Root)
{
	if (!Root) return FTopLevelAssetPath();
	FString Name;
	if ((!Root->TryGetStringField(TEXT("AssetClassPath"), Name) || UE427IsEmpty(Name)) &&
		!Root->TryGetStringField(TEXT("AssetClass"), Name)) return FTopLevelAssetPath();
	if (Name == TEXT("SimpleAsset")) {
		const TSharedPtr<FJsonObject>* Data = nullptr;
		if (!Root->TryGetObjectField(TEXT("AssetSerializedData"), Data) || !(*Data)->TryGetStringField(TEXT("AssetClass"), Name)) return FTopLevelAssetPath();
	}
	if (Name.StartsWith(TEXT("/"))) return FTopLevelAssetPath(Name);
	FTopLevelAssetPath Result;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->GetName() != Name) continue;
		const FTopLevelAssetPath Candidate(*It);
		if (!Result.IsNull() && Result != Candidate) return FTopLevelAssetPath();
		Result = Candidate;
	}
	return Result;
}
