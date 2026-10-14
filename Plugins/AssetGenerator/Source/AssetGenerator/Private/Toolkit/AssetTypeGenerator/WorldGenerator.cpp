#include "Toolkit/AssetTypeGenerator/WorldGenerator.h"
#include "UE427AssetPath.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Engine/World.h"
#include "Curves/CurveBase.h"
#include "Engine/Level.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/WorldSettings.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LevelStreaming.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeLayerInfoObject.h"
#include "Misc/FileHelper.h"
#include "Misc/SecureHash.h"
#include "UObject/MetaData.h"
#include "JsonObjectConverter.h"
#include "UObject/CoreRedirects.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionLandscapeVisibilityMask.h"

static UClass* LoadRecoveredWorldClass(const FString& Path) {
    const FString Redirected = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(Path)).ToString();
    return LoadObject<UClass>(nullptr, *Redirected, nullptr, LOAD_Quiet | LOAD_NoWarn);
}

static bool ImportLandscape(ALandscape* Landscape, const TSharedPtr<FJsonObject>& Data, const FString& BaseDirectory, FString& Error) {
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::Combine(BaseDirectory, Data->GetStringField(TEXT("Directory"))));
    const FString HeightPath = FPaths::Combine(Directory, Data->GetStringField(TEXT("HeightFile")));
    if (!FPaths::IsUnderDirectory(Directory, FPaths::ConvertRelativePathToFull(BaseDirectory)) || !FPaths::IsUnderDirectory(HeightPath, Directory)) {
        Error = TEXT("Landscape payload is outside its map dump directory."); return false;
    }
    const int32 MinX = Data->GetIntegerField(TEXT("MinX"));
    const int32 MinY = Data->GetIntegerField(TEXT("MinY"));
    const int32 MaxX = Data->GetIntegerField(TEXT("MaxX"));
    const int32 MaxY = Data->GetIntegerField(TEXT("MaxY"));
    const int32 Subsections = Data->GetIntegerField(TEXT("NumSubsections"));
    const int32 Quads = Data->GetIntegerField(TEXT("SubsectionSizeQuads"));
    const int64 Count = (int64(MaxX) - MinX + 1) * (int64(MaxY) - MinY + 1);
    if (MaxX <= MinX || MaxY <= MinY || Count > 268435456 || (Subsections != 1 && Subsections != 2) || Quads < 1 || Quads > 255 || !FMath::IsPowerOfTwo(Quads + 1)
        || (MaxX - MinX) % (Subsections * Quads) || (MaxY - MinY) % (Subsections * Quads)) {
        Error = TEXT("Invalid landscape dimensions."); return false;
    }
    TArray<uint8> Bytes;
    if (!FFileHelper::LoadFileToArray(Bytes, *HeightPath) || Bytes.Num() != Count * 2) {
        Error = FString::Printf(TEXT("Missing or invalid landscape height payload %s"), *HeightPath); return false;
    }
    const FString Hash = FMD5::HashBytes(Bytes.GetData(), Bytes.Num());
    FString PayloadSignature = FString::Printf(TEXT("%s:%d:%d:%d:%d:%d:%d"), *Hash, MinX, MinY, MaxX, MaxY, Subsections, Quads);
    for (const auto& Value : Data->GetArrayField(TEXT("Layers"))) {
        const auto Layer = Value->AsObject();
        const FString WeightPath = FPaths::Combine(Directory, Layer->GetStringField(TEXT("File")));
        TArray<uint8> Weights;
        if (!FPaths::IsUnderDirectory(WeightPath, Directory) || !FFileHelper::LoadFileToArray(Weights, *WeightPath) || Weights.Num() != Count) {
            Error = FString::Printf(TEXT("Missing or invalid landscape weight payload %s"), *WeightPath); return false;
        }
        PayloadSignature += FString::Printf(TEXT(":%s:%d:%s"), *Layer->GetStringField(TEXT("Name")), Layer->GetBoolField(TEXT("NoWeightBlend")), *FMD5::HashBytes(Weights.GetData(), Weights.Num()));
    }
    const FString PayloadHash = FMD5::HashAnsiString(*PayloadSignature);
    UMetaData* Metadata = Landscape->GetOutermost()->GetMetaData();
    if (!UE427IsEmpty(Landscape->LandscapeComponents)) {
        if (FString(Metadata->GetValue(Landscape, TEXT("CASLandscapePayloadHash"))) == PayloadHash) return true;
        Error = TEXT("Existing landscape differs from this dump or predates payload verification; refusing to replace edited terrain automatically."); return false;
    }
    TMap<FGuid, TArray<uint16>> Heights;
    TArray<uint16>& HeightData = Heights.Add(FGuid());
    HeightData.SetNumUninitialized(Count);
    FMemory::Memcpy(HeightData.GetData(), Bytes.GetData(), Bytes.Num());
    TMap<FGuid, TArray<FLandscapeImportLayerInfo>> Layers;
    TArray<FLandscapeImportLayerInfo>& LayerData = Layers.Add(FGuid());
    for (const auto& Value : Data->GetArrayField(TEXT("Layers"))) {
        const auto Layer = Value->AsObject();
        const FString Name = Layer->GetStringField(TEXT("Name"));
        const FString WeightPath = FPaths::Combine(Directory, Layer->GetStringField(TEXT("File")));
        FLandscapeImportLayerInfo Info;
        Info.LayerName = FName(*Name);
        if (!FPaths::IsUnderDirectory(WeightPath, Directory) || !FFileHelper::LoadFileToArray(Info.LayerData, *WeightPath) || Info.LayerData.Num() != Count) {
            Error = FString::Printf(TEXT("Missing or invalid landscape weight payload %s"), *WeightPath); return false;
        }
        if (Name == TEXT("Visibility")) Info.LayerInfo = ALandscapeProxy::VisibilityLayer;
        else {
            const FName ObjectName(*(Landscape->GetName() + TEXT("_Layer_") + FString::FromInt(LayerData.Num())));
            Info.LayerInfo = FindObject<ULandscapeLayerInfoObject>(Landscape->GetOutermost(), *ObjectName.ToString());
            if (!Info.LayerInfo) Info.LayerInfo = NewObject<ULandscapeLayerInfoObject>(Landscape->GetOutermost(), ObjectName, RF_Public | RF_Standalone | RF_Transactional);
            Info.LayerInfo->LayerName = Info.LayerName;
            Info.LayerInfo->bNoWeightBlend = Layer->GetBoolField(TEXT("NoWeightBlend"));
        }
        LayerData.Add(MoveTemp(Info));
    }
    FString GuidText = Data->GetStringField(TEXT("Guid")).Replace(TEXT("-"), TEXT(""));
    FGuid Guid;
    if (!FGuid::ParseExact(GuidText, EGuidFormats::Digits, Guid) || !Guid.IsValid()) { Error = TEXT("Invalid landscape GUID."); return false; }
    if (!Landscape->LandscapeMaterial) {
        const FName MaterialName(*(Landscape->GetName() + TEXT("_CASPlaceholderMaterial")));
        UMaterial* Material = FindObject<UMaterial>(Landscape->GetOutermost(), *MaterialName.ToString());
        if (!Material) {
            Material = NewObject<UMaterial>(Landscape->GetOutermost(), MaterialName, RF_Public | RF_Standalone | RF_Transactional);
            Material->BlendMode = BLEND_Masked;
            auto* Color = NewObject<UMaterialExpressionConstant3Vector>(Material);
            Color->Constant = FLinearColor(0.18f, 0.18f, 0.18f);
            auto* Visibility = NewObject<UMaterialExpressionLandscapeVisibilityMask>(Material);
            Material->Expressions.Add(Color);
            Material->Expressions.Add(Visibility);
            Material->BaseColor.Connect(0, Color);
            Material->OpacityMask.Connect(0, Visibility);
            Material->PostEditChange();
        }
        Landscape->LandscapeMaterial = Material;
        Metadata->SetValue(Material, TEXT("CASPlaceholder"), TEXT("Recovered terrain uses a neutral material with visibility holes; original cooked material graph unavailable."));
    }
    Landscape->Import(Guid, MinX, MinY, MaxX, MaxY, Subsections, Quads, Heights, *HeightPath, Layers, ELandscapeImportAlphamapType::Additive, nullptr);
    for (ULandscapeComponent* Component : Landscape->LandscapeComponents) {
        if (!Component || Component->GetMaterialInstanceCount(false) == 0) {
            Error = TEXT("Recovered landscape has no valid component material instances; refusing to save an unloadable map."); return false;
        }
    }
    Landscape->CreateLandscapeInfo();
    Metadata->SetValue(Landscape, TEXT("CASLandscapeHeightHash"), *Hash);
    Metadata->SetValue(Landscape, TEXT("CASLandscapePayloadHash"), *PayloadHash);
    Metadata->SetValue(Landscape, TEXT("CASLandscapeMaterial"), TEXT("Cooked material graph unavailable; assign reconstructed landscape material"));
    UE_LOG(LogAssetGenerator, Display, TEXT("Recovered landscape %s: %lld samples, %d components, %d paint layers"), *Landscape->GetName(), Count, Landscape->LandscapeComponents.Num(), LayerData.Num());
    return true;
}

FTopLevelAssetPath UWorldGenerator::GetAssetClass() {
    return FTopLevelAssetPath(UWorld::StaticClass());
}

void UWorldGenerator::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
    if (GetCurrentStage() == EAssetGenerationStage::DATA_POPULATION) PopulateReferencedObjectsDependencies(OutDependencies);
}

void UWorldGenerator::CreateAssetPackage() {
    UPackage* Package = CreatePackage(*GetPackageName().ToString());
    UWorld* World = UWorld::CreateWorld(EWorldType::Inactive, false, GetAssetName(), Package, false);
    if (!World) { FailGeneration(TEXT("Unable to create editor world.")); return; }
    World->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
    Package->SetPackageFlags(PKG_ContainsMap);
    SetPackageAndAsset(Package, World);
}

void UWorldGenerator::PopulateAssetWithData() {
    const TSharedPtr<FJsonObject> Data = GetAssetData();
    if (Data->GetIntegerField(TEXT("MapFormatVersion")) < 1 || Data->GetIntegerField(TEXT("MapFormatVersion")) > 2) { FailGeneration(TEXT("Unsupported CAS map format.")); return; }
    UWorld* World = GetAsset<UWorld>();
    ULevel* Level = World->PersistentLevel;
    if (!Level) { FailGeneration(TEXT("World has no persistent level.")); return; }
    UObjectHierarchySerializer* Serializer = GetObjectSerializer();
    Serializer->BindExistingObject(Data->GetIntegerField(TEXT("LevelIndex")), Level);
    const auto& Records = Data->GetArrayField(TEXT("Objects"));
    TMap<int32, UObject*> Objects;
    for (const auto& Value : Records) {
        const auto Record = Value->AsObject();
        const FString ClassPath = Record->GetStringField(TEXT("ClassPath"));
        UClass* Class = LoadRecoveredWorldClass(ClassPath);
        if (!Class || Class->HasAnyClassFlags(CLASS_Abstract)) { FailGeneration(FString::Printf(TEXT("Unsupported map class %s"), *ClassPath)); return; }
        const bool bActor = Record->GetBoolField(TEXT("IsActor"));
        if (bActor && !Class->IsChildOf(AActor::StaticClass())) {
            FailGeneration(FString::Printf(TEXT("Map actor support not implemented for %s"), *ClassPath)); return;
        }
        if (!bActor && !Class->IsChildOf(UCurveBase::StaticClass()) && Record->GetIntegerField(TEXT("Outer")) == Data->GetIntegerField(TEXT("LevelIndex"))) {
            FailGeneration(TEXT("Unexpected non-actor level object.")); return;
        }
    }
    for (const auto& Value : Records) {
        const auto Record = Value->AsObject();
        if (!Record->GetBoolField(TEXT("IsActor"))) continue;
        const int32 Index = Record->GetIntegerField(TEXT("ObjectIndex"));
        UClass* Class = LoadRecoveredWorldClass(Record->GetStringField(TEXT("ClassPath")));
        const FName Name(*Record->GetStringField(TEXT("ObjectName")));
        AActor* Actor = Class == AWorldSettings::StaticClass() ? World->GetWorldSettings() : FindObject<AActor>(Level, *Name.ToString());
        if (!Actor) {
            FActorSpawnParameters Params;
            Params.Name = Name;
            Params.OverrideLevel = Level;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            Params.ObjectFlags = RF_Transactional;
            Params.bDeferConstruction = true;
            Actor = World->SpawnActor<AActor>(Class->IsChildOf(ALandscapeProxy::StaticClass()) ? ALandscape::StaticClass() : Class, FTransform::Identity, Params);
        }
        if (!Actor || (!Actor->IsA(Class) && !Record->HasField(TEXT("Landscape")))) { FailGeneration(FString::Printf(TEXT("Cannot create map actor %s"), *Name.ToString())); return; }
        Actor->UnregisterAllComponents();
        Objects.Add(Index, Actor);
        Serializer->BindExistingObject(Index, Actor);
    }
    Objects.Add(Data->GetIntegerField(TEXT("LevelIndex")), Level);
    TArray<TSharedPtr<FJsonValue>> Pending;
    for (const auto& Value : Records) if (!Value->AsObject()->GetBoolField(TEXT("IsActor"))) Pending.Add(Value);
    while (!UE427IsEmpty(Pending)) {
        const int32 PreviousCount = Pending.Num();
        for (int32 PendingIndex = Pending.Num() - 1; PendingIndex >= 0; --PendingIndex) {
        const auto Record = Pending[PendingIndex]->AsObject();
        const int32 Index = Record->GetIntegerField(TEXT("ObjectIndex"));
        UObject* Outer = Objects.FindRef(Record->GetIntegerField(TEXT("Outer")));
        UClass* Class = LoadRecoveredWorldClass(Record->GetStringField(TEXT("ClassPath")));
        const FName Name(*Record->GetStringField(TEXT("ObjectName")));
        bool bStreaming = false;
        Record->TryGetBoolField(TEXT("IsStreamingLevel"), bStreaming);
        if (bStreaming) Outer = World;
        if (!Outer) continue;
        UObject* Object = FindObject<UObject>(Outer, *Name.ToString());
        UObject* Template = nullptr;
            if (Class->IsChildOf(UActorComponent::StaticClass())) {
                for (UClass* OwnerClass = Outer->GetClass(); OwnerClass && !Template; OwnerClass = OwnerClass->GetSuperClass()) {
                    if (UBlueprintGeneratedClass* BlueprintClass = Cast<UBlueprintGeneratedClass>(OwnerClass)) {
                        if (BlueprintClass->SimpleConstructionScript) {
                            if (USCS_Node* Node = BlueprintClass->SimpleConstructionScript->FindSCSNode(Name)) {
                                Template = Node->GetActualComponentTemplate(CastChecked<UBlueprintGeneratedClass>(Outer->GetClass()));
                            }
                        }
                        if (Template) break;
                        Template = BlueprintClass->FindComponentTemplateByName(FName(*(Name.ToString() + TEXT("_GEN_VARIABLE"))));
                        if (!Template) Template = BlueprintClass->FindComponentTemplateByName(Name);
                    }
                }
                if (!Template) Template = FindObject<UActorComponent>(Outer->GetClass()->GetDefaultObject(), *Name.ToString());
                if (Template && !Template->IsA(Class)) Template = nullptr;
            }
        if (!Object) {
            Object = NewObject<UObject>(Outer, Class, Name, RF_Transactional, Template);
        }
        if (!Object || Object->GetClass() != Class) { FailGeneration(FString::Printf(TEXT("Map object class mismatch: %s"), *Name.ToString())); return; }
        if (Template) {
            const auto& Properties = Record->GetObjectField(TEXT("Properties"));
            if (USceneComponent* Scene = Cast<USceneComponent>(Object); Scene && !Properties->HasField(TEXT("Mobility"))) Scene->SetMobility(CastChecked<USceneComponent>(Template)->Mobility);
            if (USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(Object); Mesh && !Properties->HasField(TEXT("SkinnedAsset")) && !Properties->HasField(TEXT("SkeletalMesh"))) Mesh->SetSkeletalMesh(CastChecked<USkeletalMeshComponent>(Template)->SkeletalMesh);
            if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Object); Mesh && !Properties->HasField(TEXT("StaticMesh"))) Mesh->SetStaticMesh(CastChecked<UStaticMeshComponent>(Template)->GetStaticMesh());
        }
        Objects.Add(Index, Object);
        Serializer->BindExistingObject(Index, Object);
        if (UActorComponent* Component = Cast<UActorComponent>(Object)) {
            if (AActor* Owner = Component->GetOwner(); Owner && !Component->HasAnyFlags(RF_DefaultSubObject)) Owner->AddInstanceComponent(Component);
        }
        if (ULevelStreaming* Streaming = Cast<ULevelStreaming>(Object)) World->AddStreamingLevel(Streaming);
        Pending.RemoveAt(PendingIndex);
        }
        if (Pending.Num() == PreviousCount) { FailGeneration(TEXT("Map subobjects contain missing or cyclic outers.")); return; }
    }
    for (const auto& Value : Records) {
        const auto Record = Value->AsObject();
        UObject* Object = Objects.FindChecked(Record->GetIntegerField(TEXT("ObjectIndex")));
        const auto Properties = MakeShared<FJsonObject>(*Record->GetObjectField(TEXT("Properties")));
        for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It) {
            const FStructProperty* Struct = CastField<FStructProperty>(*It);
            const auto* Value = Properties->Values.Find(It->GetName());
            if (Struct && Value && (*Value)->Type == EJson::String && (Struct->Struct->GetFName() == TEXT("SoftClassPath") || Struct->Struct->GetFName() == TEXT("SoftObjectPath"))) {
                *Struct->ContainerPtrToValuePtr<FSoftObjectPath>(Object) = FSoftObjectPath((*Value)->AsString());
                Properties->RemoveField(It->GetName());
            }
        }
        Serializer->DeserializeObjectProperties(Properties, Object);
        if (USplineComponent* Spline = Cast<USplineComponent>(Object)) {
            const FSplineCurves& Curves = Spline->SplineCurves;
            if (Curves.Position.Points.Num() != Curves.Rotation.Points.Num() || Curves.Position.Points.Num() != Curves.Scale.Points.Num()) {
                FailGeneration(FString::Printf(TEXT("Spline %s has inconsistent position, rotation and scale point counts (%d, %d, %d)."), *Spline->GetPathName(), Curves.Position.Points.Num(), Curves.Rotation.Points.Num(), Curves.Scale.Points.Num()));
                return;
            }
        }
        if (UInstancedStaticMeshComponent* Instances = Cast<UInstancedStaticMeshComponent>(Object); Instances && Record->HasField(TEXT("Instances"))) {
            Instances->ClearInstances();
            for (const auto& InstanceValue : Record->GetArrayField(TEXT("Instances"))) {
                FTransform Transform;
                if (!FJsonObjectConverter::JsonObjectToUStruct(InstanceValue->AsObject().ToSharedRef(), TBaseStructure<FTransform>::Get(), &Transform, 0, 0)) {
                    FailGeneration(TEXT("Invalid instance transform in map dump.")); return;
                }
                Instances->AddInstance(Transform);
            }
            if (Record->HasField(TEXT("InstanceCustomData"))) {
                const auto& CustomData = Record->GetArrayField(TEXT("InstanceCustomData"));
                const int32 Count = Instances->GetInstanceCount();
                if ((!Count && !UE427IsEmpty(CustomData)) || (Count && CustomData.Num() % Count)) {
                    FailGeneration(TEXT("Invalid instance custom data size in map dump.")); return;
                }
                const int32 Stride = Count ? CustomData.Num() / Count : 0;
                Instances->NumCustomDataFloats = Stride;
                for (int32 Index = 0; Index < CustomData.Num(); ++Index) {
                    Instances->SetCustomDataValue(Index / Stride, Index % Stride, CustomData[Index]->AsNumber(), false);
                }
            }
        }
    }
    for (const auto& Pair : Objects) {
        if (USceneComponent* Component = Cast<USceneComponent>(Pair.Value)) {
            USceneComponent* Parent = Component->GetAttachParent();
            if (Parent) Component->SetupAttachment(Parent, Component->GetAttachSocketName());
            Component->UpdateComponentToWorld();
        }
    }
    for (const auto& Value : Records) {
        const auto Record = Value->AsObject();
        if (Record->HasField(TEXT("Landscape"))) {
            FString Error;
            if (!ImportLandscape(CastChecked<ALandscape>(Objects.FindChecked(Record->GetIntegerField(TEXT("ObjectIndex")))), Record->GetObjectField(TEXT("Landscape")), GetPackageBaseDirectory(), Error)) {
                FailGeneration(Error); return;
            }
        }
    }
    if (Data->HasField(TEXT("RecoveryWarnings"))) {
        for (const auto& Warning : Data->GetArrayField(TEXT("RecoveryWarnings"))) UE_LOG(LogAssetGenerator, Warning, TEXT("Map recovery: %s"), *Warning->AsString());
    }
    int32 ActorCount = 0;
    for (const auto& Pair : Objects) {
        if (AActor* Actor = Cast<AActor>(Pair.Value)) { Actor->RegisterAllComponents(); ++ActorCount; }
    }
    World->UpdateWorldComponents(true, false);
    MarkAssetChanged();
    UE_LOG(LogAssetGenerator, Display, TEXT("Reconstructed map %s: %d actors, %d subobjects"), *GetPackageName().ToString(), ActorCount, Objects.Num() - ActorCount);
}
