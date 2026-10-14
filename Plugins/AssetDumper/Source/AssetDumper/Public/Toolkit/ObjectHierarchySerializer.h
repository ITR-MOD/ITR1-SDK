#pragma once
#include "UObject/Object.h"
#include "Json.h"
#include "ObjectHierarchySerializer.generated.h"

class UPropertySerializer;

/** Compare settings for one specific object */
struct ASSETDUMPER_API FObjectCompareSettings {
	/** Whenever to perform a name check on the objects */
	bool bCheckObjectName;
	/** Whenever to perform an outer check on the objects */
	bool bCheckObjectOuter;

	FObjectCompareSettings();
	FObjectCompareSettings(bool bCheckObjectName, bool bCheckObjectOuter);
};

class ASSETDUMPER_API FObjectCompareContext {
	TArray<TPair<int32, UObject*>> ObjectsAlreadyCompared;
	TMap<int32, FObjectCompareSettings> CompareSettings;
public:
	FObjectCompareContext();

	void SetObjectSettings(int32 ObjectIndex, const FObjectCompareSettings& Settings);
	bool HasObjectAlreadyBeenCompared(int32 ObjectIndex, UObject* Object);
	FObjectCompareSettings GetObjectSettings(int32 ObjectIndex) const;
};

UCLASS()
class ASSETDUMPER_API UObjectHierarchySerializer : public UObject {
    GENERATED_BODY()
private:
    UPROPERTY()
    UPackage* SourcePackage;
    UPROPERTY()
    TMap<UObject*, int32> ObjectIndices;
    UPROPERTY()
    TMap<int32, UObject*> LoadedObjects;
    int32 LastObjectIndex;
    UPROPERTY()
    UPropertySerializer* PropertySerializer;
    TMap<int32, TSharedPtr<FJsonObject>> SerializedObjects;
    UPROPERTY()
    TMap<UObject*, FString> ObjectMarks;
public:
    UObjectHierarchySerializer();

    FORCEINLINE UPropertySerializer* GetPropertySerializer() const { return PropertySerializer; }
    
    TSharedRef<FJsonObject> SerializeObjectProperties(UObject* Object);
    void SerializeObjectPropertiesIntoObject(UObject* Object, TSharedPtr<FJsonObject> OutObject);

	FORCEINLINE bool CompareUObjects(const int32 ObjectIndex, UObject* Object, bool bCheckExportName, bool bCheckExportOuter) {
		const TSharedPtr<FObjectCompareContext> Context = MakeShareable(new FObjectCompareContext);
		Context->SetObjectSettings(ObjectIndex, FObjectCompareSettings(bCheckExportName, bCheckExportOuter));
		return CompareObjectsWithContext(ObjectIndex, Object, Context);
	}
	
	bool CompareObjectsWithContext(const int32 ObjectIndex, UObject* Object, TSharedPtr<FObjectCompareContext> Context = MakeShareable(new FObjectCompareContext));
	bool AreObjectPropertiesUpToDate(const TSharedPtr<FJsonObject>& Properties, UObject* Object, const TSharedPtr<FObjectCompareContext> Context = MakeShareable(new FObjectCompareContext));

	void FlushPropertiesIntoObject(const int32 ObjectIndex, UObject* Object, bool bVerifyNameAndRename, bool bVerifyOuterAndMove);
    void DeserializeObjectProperties(const TSharedPtr<FJsonObject>& Properties, UObject* Object);

	void SetPropertySerializer(UPropertySerializer* NewPropertySerializer);
	
    void InitializeForSerialization(UPackage* NewSourcePackage);

    /**
     * Sets object mark for provided object instance
     * Instances of this object will be serialized as a simple object mark string
     * During deserialization, it is used to lookup object by mark
     */
    void SetObjectMark(UObject* Object, const FString& ObjectMark);

    void InitializeForDeserialization(const TArray<TSharedPtr<FJsonValue>>& ObjectsArray);
	void SetPackageForDeserialization(UPackage* SelfPackage);
	
    void BindExistingObject(int32 Index, UObject* Object) {
        check(Object && SerializedObjects.Contains(Index));
        if (LoadedObjects.Contains(Index)) {
                TArray<int32> BoundIndices;
                LoadedObjects.GetKeys(BoundIndices);
                for (const int32 BoundIndex : BoundIndices) {
                    int32 Outer = BoundIndex;
                    TSet<int32> Visited;
                    while (Outer != INDEX_NONE && !Visited.Contains(Outer)) {
                        if (Outer == Index) { LoadedObjects.Remove(BoundIndex); break; }
                        Visited.Add(Outer);
                        const auto* Entry = SerializedObjects.Find(Outer);
                        if (!Entry || !(*Entry)->TryGetNumberField(TEXT("Outer"), Outer)) break;
                    }
                }
        }
        LoadedObjects.Add(Index, Object);
    }

    UObject* DeserializeObject(int32 Index);
	
    int32 SerializeObject(UObject* Object);
    
    TArray<TSharedPtr<FJsonValue>> FinalizeSerialization();

	void CollectReferencedPackages(const TArray<TSharedPtr<FJsonValue>>& ReferencedSubobjects, TArray<FString>& OutReferencedPackageNames);

	void CollectReferencedPackages(const TArray<TSharedPtr<FJsonValue>>& ReferencedSubobjects, TArray<FString>& OutReferencedPackageNames, TArray<int32>& ObjectsAlreadySerialized);

	FORCEINLINE void CollectObjectPackages(const int32 ObjectIndex, TArray<FString>& OutReferencedPackageNames) {
		TArray<int32> ObjectsAlreadySerialized;
		CollectObjectPackages(ObjectIndex, OutReferencedPackageNames, ObjectsAlreadySerialized);
	}

	void CollectObjectPackages(const int32 ObjectIndex, TArray<FString>& OutReferencedPackageNames, TArray<int32>& ObjectsAlreadySerialized);

	FString GetObjectFullPath(int32 ObjectIndex);

    FORCEINLINE static const TSet<FName>& GetUnhandledNativeClasses() { return UnhandledNativeClasses; }
private:
    static TSet<FName> UnhandledNativeClasses;
    
    void SerializeImportedObject(TSharedPtr<FJsonObject> ResultJson, UObject* Object);
    void SerializeExportedObject(TSharedPtr<FJsonObject> ResultJson, UObject* Object);

    UObject* DeserializeImportedObject(TSharedPtr<FJsonObject> ObjectJson);
    UObject* DeserializeExportedObject(int32 ObjectIndex, TSharedPtr<FJsonObject> ObjectJson);
};
