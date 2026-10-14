#include "Toolkit/AssetTypeGenerator/BlueprintGenerator.h"
#include "UE427AssetPath.h"
#include "K2Node_FunctionEntry.h"
#include "Dom/JsonObject.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Toolkit/ObjectHierarchySerializer.h"
#include "Toolkit/AssetGeneration/AssetGenerationUtil.h"
#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"
#include "BlueprintCompilationManager.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionResult.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/ActorComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Animation/AnimBlueprint.h"
#include "GameFramework/Info.h"
#include "Toolkit/PropertySerializer.h"
#include "Toolkit/AssetDumping/AssetTypeSerializerMacros.h"
#include "GraphReconstruction/CASJsonGraph.h"
#include "UObject/MetaData.h"
#include "Animation/Skeleton.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/TimelineTemplate.h"

#define LOCTEXT_NAMESPACE "AssetGenerator"

static void PreparePlaceholderGraphs(UBlueprint* Blueprint, const TSharedPtr<FJsonObject>& Data) {
	FString Reason;
	if (!Data->TryGetStringField(TEXT("PlaceholderGraphReason"), Reason) || UE427IsEmpty(Reason)) return;
	Blueprint->Timelines.Reset();
	TArray<UEdGraph*> Graphs = Blueprint->FunctionGraphs;
	Graphs.Append(Blueprint->UbergraphPages);
	for (UEdGraph* Graph : Graphs) FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph, EGraphRemoveFlags::MarkTransient);
	if (FBlueprintEditorUtils::DoesSupportEventGraphs(Blueprint)) {
		UEdGraph* Graph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, TEXT("EventGraph"), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
		FBlueprintEditorUtils::AddUbergraphPage(Blueprint, Graph);
	}
	Blueprint->GetOutermost()->GetMetaData()->SetValue(Blueprint, TEXT("CASPlaceholder"), *Reason);
	Blueprint->GetOutermost()->GetMetaData()->RemoveValue(Blueprint, TEXT("CASGraphSource"));
}

static void ClearRecoveredGraphPlaceholder(UBlueprint* Blueprint) {
	UMetaData* Metadata = Blueprint->GetOutermost()->GetMetaData();
	if (Blueprint->Status == BS_Error || UE427IsEmpty(Metadata->GetValue(Blueprint, TEXT("CASGraphSource")))) return;
	if (Metadata->GetValue(Blueprint, TEXT("CASPlaceholder")).StartsWith(TEXT("Level recovery: graph logic unavailable;"))) {
		Metadata->RemoveValue(Blueprint, TEXT("CASPlaceholder"));
		Blueprint->MarkPackageDirty();
	}
}

static bool RestoreCookedSkeletalMeshTemplates(UBlueprint* Blueprint, UObjectHierarchySerializer* Serializer, int32 DefaultObjectIndex) {
	const auto Objects = Serializer->FinalizeSerialization();
	bool bTemplatesChanged = false;
	for (const auto& Value : Objects) {
		const auto Entry = Value->AsObject();
		int32 OuterIndex = INDEX_NONE;
		const TSharedPtr<FJsonObject>* ComponentProperties = nullptr;
		if (!Entry->TryGetNumberField(TEXT("Outer"), OuterIndex) || OuterIndex != DefaultObjectIndex ||
			!Entry->TryGetObjectField(TEXT("Properties"), ComponentProperties)) continue;
		int32 MeshIndex = INDEX_NONE;
		const bool bStaticMesh = (*ComponentProperties)->HasField(TEXT("StaticMesh"));
		const bool bInheritMesh = !(*ComponentProperties)->TryGetNumberField(TEXT("SkinnedAsset"), MeshIndex) &&
			!(*ComponentProperties)->TryGetNumberField(TEXT("SkeletalMesh"), MeshIndex) &&
			!(*ComponentProperties)->TryGetNumberField(TEXT("StaticMesh"), MeshIndex);
		UObject* Mesh = MeshIndex == INDEX_NONE ? nullptr : Serializer->DeserializeObject(MeshIndex);
		if (Mesh && (bStaticMesh ? !Mesh->IsA<UStaticMesh>() : !Mesh->IsA<USkeletalMesh>())) continue;
		if (MeshIndex != INDEX_NONE && !Mesh) {
			UE_LOG(LogAssetGenerator, Warning, TEXT("Cannot restore mesh for %s.%s: mesh reference %d is unresolved"), *Blueprint->GetPathName(), *Entry->GetStringField(TEXT("ObjectName")), MeshIndex);
			continue;
		}
		const FName ComponentName(*Entry->GetStringField(TEXT("ObjectName")));
		USCS_Node* FoundNode = nullptr;
		UBlueprintGeneratedClass* DeclaringClass = nullptr;
		for (UClass* Class = Blueprint->GeneratedClass; Class && !FoundNode; Class = Class->GetSuperClass()) {
			auto GeneratedClass = Cast<UBlueprintGeneratedClass>(Class);
			if (!GeneratedClass || !GeneratedClass->SimpleConstructionScript) continue;
			for (USCS_Node* Node : GeneratedClass->SimpleConstructionScript->GetAllNodes()) {
				if (Node && Node->ComponentTemplate && (Node->GetVariableName() == ComponentName || Node->ComponentTemplate->GetFName() == ComponentName)) {
					FoundNode = Node;
					DeclaringClass = GeneratedClass;
					break;
				}
			}
		}
		auto Template = FoundNode ? Cast<UMeshComponent>(FoundNode->GetActualComponentTemplate(CastChecked<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))) :
			Cast<UMeshComponent>(Blueprint->GeneratedClass->GetDefaultObject()->GetDefaultSubobjectByName(ComponentName));
		if (!Template) continue;
		if (bInheritMesh) {
			auto ParentClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass->GetSuperClass());
			if (!ParentClass || (FoundNode && DeclaringClass == Blueprint->GeneratedClass)) continue;
			auto ParentTemplate = FoundNode ? Cast<UMeshComponent>(FoundNode->GetActualComponentTemplate(ParentClass)) : Cast<UMeshComponent>(ParentClass->GetDefaultObject()->GetDefaultSubobjectByName(ComponentName));
			if (auto Skinned = Cast<USkeletalMeshComponent>(ParentTemplate)) Mesh = Skinned->SkeletalMesh;
			else if (auto Static = Cast<UStaticMeshComponent>(ParentTemplate)) Mesh = Static->GetStaticMesh();
			else continue;
		}
		if (auto Skeletal = Cast<USkeletalMeshComponent>(Template)) { if (Skeletal->SkeletalMesh == Mesh) continue; }
		else if (auto Static = Cast<UStaticMeshComponent>(Template)) { if (Static->GetStaticMesh() == Mesh) continue; }
		else continue;
		if (FoundNode && DeclaringClass != Blueprint->GeneratedClass) {
			auto Handler = CastChecked<UBlueprintGeneratedClass>(Blueprint->GeneratedClass)->GetInheritableComponentHandler(true);
			const FComponentKey Key(FoundNode);
			Template = Cast<UMeshComponent>(Handler->GetOverridenComponentTemplate(Key));
			if (!Template) Template = Cast<UMeshComponent>(Handler->CreateOverridenComponentTemplate(Key));
		}
		if (!Template) continue;
		Template->Modify();
		if (auto Skeletal = Cast<USkeletalMeshComponent>(Template)) Skeletal->SetSkeletalMesh(Cast<USkeletalMesh>(Mesh));
		else if (auto Static = Cast<UStaticMeshComponent>(Template)) Static->SetStaticMesh(Cast<UStaticMesh>(Mesh));
		bTemplatesChanged = true;
		UE_LOG(LogAssetGenerator, Display, TEXT("Restored mesh template %s.%s = %s"), *Blueprint->GetPathName(), *ComponentName.ToString(), *GetPathNameSafe(Mesh));
	}
	if (bTemplatesChanged) {
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		FBlueprintCompilationManager::CompileSynchronously(FBPCompileRequest(Blueprint, EBlueprintCompileOptions::SkipDefaultObjectValidation | EBlueprintCompileOptions::SkipGarbageCollection, nullptr));
		Blueprint->MarkPackageDirty();
	}
	return bTemplatesChanged;
}

bool UBlueprintGenerator::RebuildExistingGraphs(UBlueprint* Blueprint, FString& OutError, bool bDeclarationsOnly, bool bDefaultsOnly) {
	if (!Blueprint || !Blueprint->GeneratedClass || (!bDeclarationsOnly && Blueprint->IsA<UAnimBlueprint>())) {
		OutError = TEXT("Graph repair requires an existing non-animation Blueprint with a generated class.");
		return false;
	}
	SetPackageAndAsset(Blueprint->GetOutermost(), Blueprint, false);
	UpdateDeserializerBlueprintClassObject(false);
	if (bDefaultsOnly) {
		FBlueprintGeneratorUtils::RestoreClassDefaults(Blueprint, GetObjectSerializer(), GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject")));
		return Blueprint->Status != BS_Error;
	}
	if (bDeclarationsOnly) {
		if (auto AnimationBlueprint = Cast<UAnimBlueprint>(Blueprint)) {
			int32 SkeletonIndex = INDEX_NONE;
			if (GetAssetObjectData()->TryGetNumberField(TEXT("TargetSkeleton"), SkeletonIndex)) {
				if (auto Skeleton = Cast<USkeleton>(GetObjectSerializer()->DeserializeObject(SkeletonIndex))) AnimationBlueprint->TargetSkeleton = Skeleton;
			}
			else if (!AnimationBlueprint->TargetSkeleton && GetAssetData()->GetArrayField(TEXT("ChildProperties")).ContainsByPredicate([](const TSharedPtr<FJsonValue>& Value) {
				return Value->AsObject()->GetStringField(TEXT("ObjectName")).StartsWith(TEXT("AnimGraphNode_LinkedInputPose"));
			})) {
				(void)0;
				UE_LOG(LogAssetGenerator, Display, TEXT("Restoring skeleton-independent animation template declarations for %s; pose graph reconstruction is separate."), *Blueprint->GetPathName());
			}
		}
	}
	bool bMissingComponent = false;
	for (const auto& Value : GetAssetData()->GetArrayField(TEXT("ChildProperties"))) {
		const auto Property = Value->AsObject();
		if (Property->GetStringField(TEXT("ObjectClass")) == TEXT("ObjectProperty") &&
			!FindFProperty<FProperty>(Blueprint->GeneratedClass, *Property->GetStringField(TEXT("ObjectName")))) bMissingComponent = true;
	}
	int32 ScriptIndex = INDEX_NONE;
	if ((bMissingComponent || bDeclarationsOnly) && GetAssetObjectData()->TryGetNumberField(TEXT("SimpleConstructionScript"), ScriptIndex) && ScriptIndex != INDEX_NONE) {
		if (auto Script = Cast<USimpleConstructionScript>(GetObjectSerializer()->DeserializeObject(ScriptIndex))) {
			Blueprint->SimpleConstructionScript = Script;
			Script->SetFlags(RF_Transactional);
			UpdateDeserializerBlueprintClassObject(true);
		}
	}
	PopulateAssetWithData();
	if (bDeclarationsOnly) {
		UpdateDeserializerBlueprintClassObject(true);
		if (Blueprint->Status == BS_Error) OutError = TEXT("Restored Blueprint declarations failed compilation.");
		if (UE427IsEmpty(OutError)) FBlueprintGeneratorUtils::RestoreClassDefaults(Blueprint, GetObjectSerializer(), GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject")));
		return UE427IsEmpty(OutError);
	}
	Blueprint->GetOutermost()->GetMetaData()->RemoveValue(Blueprint, TEXT("CASGraphSource"));
	const bool bBuilt = FCASJsonGraph::Generate(Blueprint, GetAssetData(), GetObjectSerializer(), OutError);
	if (!UE427IsEmpty(OutError)) return false;
	if (bBuilt) {
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		UpdateDeserializerBlueprintClassObject(true);
	}
	if (Blueprint->Status == BS_Error) {
		OutError = TEXT("Reconstructed Blueprint failed compilation.");
		return false;
	}
	ClearRecoveredGraphPlaceholder(Blueprint);
	FBlueprintGeneratorUtils::RestoreClassDefaults(Blueprint, GetObjectSerializer(), GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject")));
	return true;
}

bool FBlueprintGeneratorUtils::RestoreClassDefaults(UBlueprint* Blueprint, UObjectHierarchySerializer* Serializer, int32 DefaultObjectIndex) {
	if (!Blueprint || !Blueprint->GeneratedClass || !Serializer) return false;
	const auto Objects = Serializer->FinalizeSerialization();
	if (!Objects.IsValidIndex(DefaultObjectIndex)) return false;
	const TSharedPtr<FJsonObject>* Properties = nullptr;
	if (!Objects[DefaultObjectIndex]->AsObject()->TryGetObjectField(TEXT("Properties"), Properties)) return false;
	const bool bTemplatesChanged = RestoreCookedSkeletalMeshTemplates(Blueprint, Serializer, DefaultObjectIndex);
	Serializer->SetObjectMark(Blueprint->GeneratedClass, TEXT("$AssetObject$"));
	UObject* DefaultObject = Blueprint->GeneratedClass->GetDefaultObject();
	Serializer->BindExistingObject(DefaultObjectIndex, DefaultObject);
	const auto Context = MakeShared<FObjectCompareContext>();
	Context->SetObjectSettings(DefaultObjectIndex, FObjectCompareSettings(false, false));
	if (Serializer->AreObjectPropertiesUpToDate(*Properties, DefaultObject, Context)) return bTemplatesChanged;
	DefaultObject->Modify();
	Serializer->DeserializeObjectProperties(*Properties, DefaultObject);
	Blueprint->MarkPackageDirty();
	return true;
}

UBlueprint* UBlueprintGenerator::CreateNewBlueprint(UPackage* Package, UClass* ParentClass) {
	EBlueprintType BlueprintType = BPTYPE_Normal;

	if (ParentClass->HasAnyClassFlags(CLASS_Const)) {
		BlueprintType = BPTYPE_Const;
	}
	if (ParentClass == UBlueprintFunctionLibrary::StaticClass()) {
		BlueprintType = BPTYPE_FunctionLibrary;
	}
	if (ParentClass == UInterface::StaticClass()) {
		BlueprintType = BPTYPE_Interface;
	}
	return FKismetEditorUtilities::CreateBlueprint(ParentClass, Package, GetAssetName(), BlueprintType, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}

void UBlueprintGenerator::CreateAssetPackage() {
	const int32 SuperStructIndex = GetAssetData()->GetIntegerField(TEXT("SuperStruct"));
	UClass* ParentClass = Cast<UClass>(GetObjectSerializer()->DeserializeObject(SuperStructIndex));
	
	if (ParentClass == NULL) {
		const FString ObjectName = GetObjectSerializer()->GetObjectFullPath(SuperStructIndex);
		
		const FText MessageRaw = LOCTEXT("ParentClassNotFoundForBlueprint", "Cannot resolve parent class {ParentClass} for blueprint {Blueprint}");
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("Blueprint"), FText::FromName(GetPackageName()));
		Arguments.Add(TEXT("ParentClass"), FText::FromString(*ObjectName));
		
		if (!FApp::IsUnattended()) {
			FMessageDialog::Open(EAppMsgType::Ok, FText::Format(MessageRaw, Arguments));
		}
		ParentClass = GetFallbackParentClass();
	}

	UPackage* NewPackage = CreatePackage(*GetPackageName().ToString());
	UBlueprint* NewBlueprint = CreateNewBlueprint(NewPackage, ParentClass);
	SetPackageAndAsset(NewPackage, NewBlueprint, false);

	UpdateDeserializerBlueprintClassObject(false);
	MarkAssetChanged();
	
	PostConstructOrUpdateAsset(NewBlueprint);
}

void UBlueprintGenerator::OnExistingPackageLoaded() {
	UBlueprint* Blueprint = GetAsset<UBlueprint>();

	const int32 SuperStructIndex = GetAssetData()->GetIntegerField(TEXT("SuperStruct"));
	UClass* ParentClass = Cast<UClass>(GetObjectSerializer()->DeserializeObject(SuperStructIndex));

	//Make sure blueprint has correct parent class, and if it doesn't re-parent it, hopefully without causing too many issues
	if (ParentClass != NULL && Blueprint->ParentClass != ParentClass) {
		UE_LOG(LogAssetGenerator, Warning, TEXT("Reparenting Blueprint %s (%s -> %s)"), *Blueprint->GetPathName(), *Blueprint->ParentClass->GetPathName(), *ParentClass->GetPathName());

		FBlueprintGeneratorUtils::EnsureBlueprintUpToDate(Blueprint);
		Blueprint->ParentClass = ParentClass;
		FBlueprintGeneratorUtils::EnsureBlueprintUpToDate(Blueprint);
		
		FBlueprintCompilationManager::CompileSynchronously(FBPCompileRequest(Blueprint, EBlueprintCompileOptions::SkipDefaultObjectValidation | EBlueprintCompileOptions::SkipGarbageCollection, NULL));
		MarkAssetChanged();
	}
	
	PostConstructOrUpdateAsset(Blueprint);
}

void UBlueprintGenerator::PostInitializeAssetGenerator() {
	UPropertySerializer* Serializer = GetPropertySerializer();

	// Only exist in editor
	DISABLE_SERIALIZATION_RAW(AInfo, "SpriteComponent");
	DISABLE_SERIALIZATION_RAW(AInfo, "RootComponent");
}

void UBlueprintGenerator::PostConstructOrUpdateAsset(UBlueprint* Blueprint) {
 bool bFlagsChanged = false;
	EClassFlags ClassFlags = (EClassFlags) FCString::Atoi64(*GetAssetData()->GetStringField(TEXT("ClassFlags")));
	
	const bool bGenerateConstClass = (ClassFlags & CLASS_Const) != 0;
	const bool bGenerateAbstractClass = (ClassFlags & CLASS_Abstract) != 0;
	const bool bDeprecatedClass = (ClassFlags & CLASS_Deprecated) != 0;

	//Update blueprint flags by looking at the generated class flags
	if (bGenerateConstClass != Blueprint->bGenerateConstClass) {
		bFlagsChanged = true; Blueprint->bGenerateConstClass = bGenerateConstClass;
		MarkAssetChanged();
	}
	if (bGenerateAbstractClass != Blueprint->bGenerateAbstractClass) {
		bFlagsChanged = true; Blueprint->bGenerateAbstractClass = bGenerateAbstractClass;
		MarkAssetChanged();
	}
	if (bDeprecatedClass != Blueprint->bDeprecate) {
		bFlagsChanged = true; Blueprint->bDeprecate = bDeprecatedClass;
		MarkAssetChanged();
	}

	//Recompile blueprint if asset has actually been changed
	UpdateDeserializerBlueprintClassObject(bFlagsChanged);
}

void UBlueprintGenerator::PopulateAssetWithData() {
	UpdateDeserializerBlueprintClassObject(false);
	
	UBlueprint* Blueprint = GetAsset<UBlueprint>();
	PreparePlaceholderGraphs(Blueprint, GetAssetData());
	
	//Make sure blueprint implements are required interfaces
	TArray<TSharedPtr<FJsonValue>> ImplementedInterfaces = GetAssetData()->GetArrayField(TEXT("Interfaces"));
	bool bInterfaceChanged = false;
	for (int32 i = 0; i < ImplementedInterfaces.Num(); i++) {
		const TSharedPtr<FJsonObject> InterfaceObject = ImplementedInterfaces[i]->AsObject();
		const int32 ClassObjectIndex = InterfaceObject->GetIntegerField(TEXT("Class"));

		UClass* InterfaceClass = CastChecked<UClass>(GetObjectSerializer()->DeserializeObject(ClassObjectIndex));
		if (FBlueprintGeneratorUtils::ImplementNewInterface(Blueprint, InterfaceClass)) {
			bInterfaceChanged = true;
		}
	}
	if (bInterfaceChanged) {
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}

	const TArray<TSharedPtr<FJsonValue>>& ChildProperties = GetAssetData()->GetArrayField(TEXT("ChildProperties"));
	const TArray<TSharedPtr<FJsonValue>>& Children = GetAssetData()->GetArrayField(TEXT("Children"));
	const TArray<TSharedPtr<FJsonValue>>& GeneratedVariablesArray = GetAssetData()->GetArrayField(TEXT("GeneratedVariableNames"));

	TSet<FName> GeneratedVariableNames;
	for (const TSharedPtr<FJsonValue>& VariableName : GeneratedVariablesArray) {
		GeneratedVariableNames.Add(*VariableName->AsString());
	}
	
	TArray<FDeserializedProperty> Properties;
	TMap<FName, FDeserializedFunction> FunctionMap;

	for (int32 i = 0; i < ChildProperties.Num(); i++) {
		const TSharedPtr<FJsonObject> PropertyObject = ChildProperties[i]->AsObject();
		if (Blueprint->IsA<UAnimBlueprint>() && GeneratedVariableNames.Contains(FName(*PropertyObject->GetStringField(TEXT("ObjectName"))))) {
			continue;
		}
		new (Properties) FDeserializedProperty(PropertyObject, GetObjectSerializer());
	}

	for (int32 i = 0; i < Children.Num(); i++) {
		const TSharedPtr<FJsonObject> FunctionObject = Children[i]->AsObject();
		FDeserializedFunction DeserializedFunction(FunctionObject, GetObjectSerializer(), true);
		FunctionMap.Add(DeserializedFunction.FunctionName, MoveTemp(DeserializedFunction));
	}

	TArray<FDeserializedFunction> Functions;
	FunctionMap.GenerateValueArray(Functions);


	//Regenerate blueprint properties
	const bool bChangedProperties = FBlueprintGeneratorUtils::CreateBlueprintVariablesForProperties(Blueprint, Properties, FunctionMap,
		[&](const FDeserializedProperty& Property){
		return !GeneratedVariableNames.Contains(Property.PropertyName);
	});

	//Force blueprint compilation after we have changed any properties
	if (bChangedProperties) {
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}

	//Rebuild any functions that blueprint has
	const bool bChangedFunctions = FBlueprintGeneratorUtils::CreateNewBlueprintFunctions(Blueprint, Functions,
		[&](const FDeserializedFunction& Function){
			return true;
	}, true);

	//Recompile blueprint so all the function changed are visible
	if (bChangedFunctions) {
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}
}

void UBlueprintGenerator::FinalizeAssetCDO() {
	UpdateDeserializerBlueprintClassObject(false);
	
	UBlueprint* Blueprint = GetAsset<UBlueprint>();
	USimpleConstructionScript* OldSimpleConstructionScript = Blueprint->SimpleConstructionScript;
	
	//TEMPFIX: Move SCS to the correct outer if it does not have one already
	if (OldSimpleConstructionScript != NULL && OldSimpleConstructionScript->GetOuter() != Blueprint->GeneratedClass) {
		OldSimpleConstructionScript->Rename(NULL, Blueprint->GeneratedClass, REN_DoNothing);
	}

	//Flush CDO changes into the existing class default object
	UObject* ClassDefaultObject = Blueprint->GeneratedClass->GetDefaultObject();
	const int32 ClassDefaultObjectIndex = GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject"));
	const bool bCDOChanged = !GetObjectSerializer()->CompareUObjects(
		ClassDefaultObjectIndex, ClassDefaultObject, false, false);
	
	if (ClassDefaultObject && bCDOChanged) {
		GetObjectSerializer()->FlushPropertiesIntoObject(ClassDefaultObjectIndex, ClassDefaultObject, false, false);

		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}

	//Flush SimpleConstructionScript changes too
	const TSharedPtr<FJsonObject> AssetObjectData = GetAssetData()->GetObjectField(TEXT("AssetObjectData"));
	TSharedRef<FJsonObject> TemplateData = MakeShared<FJsonObject>();
	for (const TCHAR* Field : {TEXT("ComponentTemplates"), TEXT("InheritableComponentHandler")}) {
		if (const TSharedPtr<FJsonValue> Value = AssetObjectData->TryGetField(Field)) TemplateData->SetField(Field, Value);
	}
	if (!UE427IsEmpty(TemplateData->Values) && !GetObjectSerializer()->AreObjectPropertiesUpToDate(TemplateData, Blueprint)) {
		GetObjectSerializer()->DeserializeObjectProperties(TemplateData, Blueprint);
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}
	const int32 SimpleConstructionScriptIndex = AssetObjectData->GetIntegerField(TEXT("SimpleConstructionScript"));
	
	const bool bScriptObjectChanged = !GetObjectSerializer()->CompareUObjects(
		SimpleConstructionScriptIndex, OldSimpleConstructionScript, false, false);
	
	if (bScriptObjectChanged && SimpleConstructionScriptIndex != INDEX_NONE) {
		//Trash out old SimpleConstructionScript so we can straight up replace it with the new one
		if(Blueprint->SimpleConstructionScript != NULL) {
			MoveToTransientPackageAndRename(Blueprint->SimpleConstructionScript);
		}

		//Deserialize new SCS, update the flags accordingly and assign it to the blueprint
		//There is no need to duplicate it because it's owner is actually supposed to be the BPGC (for whatever reason)
		UObject* SimpleConstructionScript = GetObjectSerializer()->DeserializeObject(SimpleConstructionScriptIndex);
		SimpleConstructionScript->SetFlags(RF_Transactional);
		Blueprint->SimpleConstructionScript = CastChecked<USimpleConstructionScript>(SimpleConstructionScript);
		
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}
	if (RestoreCookedSkeletalMeshTemplates(Blueprint, GetObjectSerializer(), GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject")))) {
		MarkAssetChanged();
	}
}

void UBlueprintGenerator::PreFinishAssetGeneration() {
	UBlueprint* Blueprint = GetAsset<UBlueprint>();
	FString GraphError;
	const bool bGeneratedGraphs = FCASJsonGraph::Generate(Blueprint, GetAssetData(), GetObjectSerializer(), GraphError);
	if (!UE427IsEmpty(GraphError)) {
		FailGeneration(GraphError);
		return;
	}
	if (bGeneratedGraphs)
	{

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		UpdateDeserializerBlueprintClassObject(true);
		MarkAssetChanged();
	}
	if (Blueprint->Status == BS_Error) {
		FailGeneration(FString::Printf(TEXT("Reconstructed blueprint %s failed compilation"), *Blueprint->GetPathName()));
	}
	else if (!UE427IsEmpty(GraphError)) {
		FailGeneration(GraphError);
	}
	else if (FBlueprintGeneratorUtils::RestoreClassDefaults(Blueprint, GetObjectSerializer(), GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject")))) {
		MarkAssetChanged();
	}
	ClearRecoveredGraphPlaceholder(Blueprint);
}

void UBlueprintGenerator::UpdateDeserializerBlueprintClassObject(bool bRecompileBlueprint) {
	UBlueprint* Blueprint = GetAsset<UBlueprint>();
	if (bRecompileBlueprint) {
		if (Blueprint->SimpleConstructionScript) {
			for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes()) {
				if (!Node || Node->ComponentTemplate || !Node->ComponentClass || !Node->ComponentClass->IsChildOf(UActorComponent::StaticClass())) continue;
				Node->ComponentTemplate = NewObject<UActorComponent>(Blueprint->GeneratedClass, Node->ComponentClass,
					MakeUniqueObjectName(Blueprint->GeneratedClass, Node->ComponentClass, *(Node->GetVariableName().ToString() + TEXT("_GEN_VARIABLE"))), RF_ArchetypeObject | RF_Public | RF_Transactional);
				Node->ComponentTemplate->bIsEditorOnly = Node->GetVariableName().ToString().StartsWith(TEXT("EditorOnly_"));
				UE_LOG(LogAssetGenerator, Warning, TEXT("Restored stripped component template %s.%s with class defaults."), *Blueprint->GetPathName(), *Node->GetVariableName().ToString());
			}
		}
		FBlueprintCompilationManager::CompileSynchronously(FBPCompileRequest(Blueprint, EBlueprintCompileOptions::SkipDefaultObjectValidation | EBlueprintCompileOptions::SkipGarbageCollection, NULL));
	}

	UClass* BlueprintGeneratedClass = Blueprint->GeneratedClass;
	GetObjectSerializer()->SetObjectMark(CastChecked<UClass>(BlueprintGeneratedClass), TEXT("$AssetObject$"));
}

UClass* UBlueprintGenerator::GetFallbackParentClass() const {
	return AActor::StaticClass();
}

void UBlueprintGenerator::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
	if (GetAssetName().ToString().Contains(TEXT("MinerMk1"))) {
		// __debugbreak();
	}
	if (GetCurrentStage() == EAssetGenerationStage::CONSTRUCTION) {
		//For construction we want parent class to be FULLY generated
		TArray<FString> ReferencedPackages;
		
		const int32 SuperStructIndex = GetAssetData()->GetIntegerField(TEXT("SuperStruct"));
		GetObjectSerializer()->CollectObjectPackages(SuperStructIndex, ReferencedPackages);

		for (const FString& PackageName : ReferencedPackages) {
			OutDependencies.Add(FPackageDependency{*PackageName, EAssetGenerationStage::CDO_FINALIZATION});
		}
	}
	
	if (GetCurrentStage() == EAssetGenerationStage::DATA_POPULATION) {
		TArray<FDependency> AllDependencies;
		{
			//We need interfaces to be fully ready
			TArray<FString> ReferencedPackages;
			TArray<TSharedPtr<FJsonValue>> ImplementedInterfaces = GetAssetData()->GetArrayField(TEXT("Interfaces"));

			for (int32 i = 0; i < ImplementedInterfaces.Num(); i++) {
				const TSharedPtr<FJsonObject> InterfaceObject = ImplementedInterfaces[i]->AsObject();
				const int32 ClassObjectIndex = InterfaceObject->GetIntegerField(TEXT("Class"));
				GetObjectSerializer()->CollectObjectPackages(ClassObjectIndex, ReferencedPackages);
			}

			for (const FString& PackageName : ReferencedPackages) {
				AllDependencies.Add({*PackageName, false});
			}
		}
		
		//For data population we want to have all objects referenced by properties fully constructed
		
		const TArray<TSharedPtr<FJsonValue>>& ChildProperties = GetAssetData()->GetArrayField(TEXT("ChildProperties"));
		const TArray<TSharedPtr<FJsonValue>>& Children = GetAssetData()->GetArrayField(TEXT("Children"));
		
		for (const TSharedPtr<FJsonValue>& PropertyPtr : ChildProperties) {
			const TSharedPtr<FJsonObject> PropertyObject = PropertyPtr->AsObject();
			check(PropertyObject->GetStringField(TEXT("FieldKind")) == TEXT("Property"));
			
			FAssetGenerationUtil::GetPropertyDependencies(PropertyObject, GetObjectSerializer(), AllDependencies);
		}

		for (int32 i = 0; i < Children.Num(); i++) {
			const TSharedPtr<FJsonObject> FunctionObject = Children[i]->AsObject();
			check(FunctionObject->GetStringField(TEXT("FieldKind")) == TEXT("Function"));
			
			const TArray<TSharedPtr<FJsonValue>> FunctionProperties = FunctionObject->GetArrayField(TEXT("ChildProperties"));
			
			for (int32 j = 0; j < FunctionProperties.Num(); j++) {
				const TSharedPtr<FJsonObject> FunctionProperty = FunctionProperties[j]->AsObject();
				check(FunctionProperty->GetStringField(TEXT("FieldKind")) == TEXT("Property"));
			
				if (FAssetGenerationUtil::IsFunctionSignatureRelevantProperty(FunctionProperty)) {
					FAssetGenerationUtil::GetPropertyDependencies(FunctionProperty, GetObjectSerializer(), AllDependencies);
				}
			}
		}
		for (const FDependency& Dependency : AllDependencies) {
			
			OutDependencies.Add(FPackageDependency{
				*Dependency.Name,
				Dependency.bAllowPartialClass
				? EAssetGenerationStage::CONSTRUCTION
				: EAssetGenerationStage::PRE_FINSHED});
		}
	}

	if (GetCurrentStage() == EAssetGenerationStage::PRE_FINSHED) {
		TArray<FString> GraphPackages;
		FCASJsonGraph::CollectDependencies(GetAssetData(), GraphPackages);
		TArray<FDependency> LocalPropertyDependencies;
		for (const auto& FunctionValue : GetAssetData()->GetArrayField(TEXT("Children")))
		{
			const TArray<TSharedPtr<FJsonValue>>* LocalProperties = nullptr;
			if (FunctionValue->AsObject()->TryGetArrayField(TEXT("ChildProperties"), LocalProperties))
				for (const auto& Property : *LocalProperties) FAssetGenerationUtil::GetPropertyDependencies(Property->AsObject(), GetObjectSerializer(), LocalPropertyDependencies);
		}
		for (const auto& Dependency : LocalPropertyDependencies) GraphPackages.AddUnique(Dependency.Name);
		for (const FString& PackageName : GraphPackages)
			if (FName(*PackageName) != GetPackageName()) OutDependencies.Add(FPackageDependency{*PackageName, EAssetGenerationStage::CDO_FINALIZATION});
	}
	if (GetCurrentStage() == EAssetGenerationStage::CDO_FINALIZATION) {
		//For CDO finalization we want to have pretty much everything resolved, primarily CDO and SimpleConstructionScript
		const int32 CDOIndex = GetAssetData()->GetIntegerField(TEXT("ClassDefaultObject"));
		const int32 SCSIndex = GetAssetData()->GetObjectField(TEXT("AssetObjectData"))->GetIntegerField(TEXT("SimpleConstructionScript"));

		TArray<FString> AllDependencyNames;
		GetObjectSerializer()->CollectObjectPackages(CDOIndex, AllDependencyNames);
		GetObjectSerializer()->CollectObjectPackages(SCSIndex, AllDependencyNames);
		const auto ObjectData = GetAssetData()->GetObjectField(TEXT("AssetObjectData"));
		int32 HandlerIndex = INDEX_NONE;
		if (ObjectData->TryGetNumberField(TEXT("InheritableComponentHandler"), HandlerIndex))
			GetObjectSerializer()->CollectObjectPackages(HandlerIndex, AllDependencyNames);
		const TArray<TSharedPtr<FJsonValue>>* Templates = nullptr;
		if (ObjectData->TryGetArrayField(TEXT("ComponentTemplates"), Templates))
			GetObjectSerializer()->CollectReferencedPackages(*Templates, AllDependencyNames);
		
		for (const FString& PackageName : AllDependencyNames) {
        	OutDependencies.Add(FPackageDependency{*PackageName, EAssetGenerationStage::CONSTRUCTION});
        }
	}
}

FTopLevelAssetPath UBlueprintGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UBlueprint::StaticClass());
}

//Never generate __DelegateSignature methods, they are automatically handled
//ExecuteUbergraph methods should also never be generated, since they have corresponding function entries
bool FBlueprintGeneratorUtils::IsFunctionNameGenerated(const FString& FunctionName) {
	return FunctionName.EndsWith(HEADER_GENERATED_DELEGATE_SIGNATURE_SUFFIX) ||
		FunctionName.StartsWith(UEdGraphSchema_K2::FN_ExecuteUbergraphBase.ToString()) ||
		FunctionName.StartsWith(TEXT("SequenceEvent__ENTRYPOINT")) ||
		FunctionName.StartsWith(TEXT("BndEvt__")) ||
		FunctionName.StartsWith(TEXT("InpActEvt_"));
}

//UberGraphFrame properties should never be generated
bool FBlueprintGeneratorUtils::IsPropertyNameGenerated(const FString& PropertyName) {
	return PropertyName == TEXT("UberGraphFrame");
}

void FBlueprintGeneratorUtils::EnsureBlueprintUpToDate(UBlueprint* Blueprint) {
	// Purge any NULL graphs
	FBlueprintEditorUtils::PurgeNullGraphs(Blueprint);

	// Make sure the blueprint is cosmetically up to date
	FKismetEditorUtilities::UpgradeCosmeticallyStaleBlueprint(Blueprint);

	// Make sure that this blueprint is up-to-date with regards to its parent functions
	FBlueprintEditorUtils::ConformCallsToParentFunctions(Blueprint);

	// Make sure that this blueprint is up-to-date with regards to its implemented events
	FBlueprintEditorUtils::ConformImplementedEvents(Blueprint);

	// Make sure that this blueprint is up-to-date with regards to its implemented interfaces
	FBlueprintEditorUtils::ConformImplementedInterfaces(Blueprint);

	// Update old composite nodes(can't do this in PostLoad)
	FBlueprintEditorUtils::UpdateOutOfDateCompositeNodes(Blueprint);

	// Update any nodes which might have dropped their RF_Transactional flag due to copy-n-paste issues
	FBlueprintEditorUtils::UpdateTransactionalFlags(Blueprint);
}

bool FBlueprintGeneratorUtils::ImplementNewInterface(UBlueprint* Blueprint, UClass* InterfaceClass) {
	
	//Check to make sure we haven't already implemented it
	for(int32 i = 0; i < Blueprint->ImplementedInterfaces.Num(); i++) {
		if(Blueprint->ImplementedInterfaces[i].Interface == InterfaceClass) {
			return false;
		}
	}

	//Make sure it is also not implemented by the parent class
	if (Blueprint->ParentClass->ImplementsInterface(InterfaceClass)) {
		return false;
	}

	// Make a new entry for this interface
	FBPInterfaceDescription NewInterface;
	NewInterface.Interface = InterfaceClass;

	// Add the graphs for the functions required by this interface
	for(TFieldIterator<UFunction> FunctionIter(InterfaceClass, EFieldIteratorFlags::IncludeSuper); FunctionIter; ++FunctionIter) {
		UFunction* Function = *FunctionIter;
		const bool bIsAnimFunction = Function->HasMetaData(FBlueprintMetadata::MD_AnimBlueprintFunction) && Blueprint->IsA<UAnimBlueprint>();
		const bool bShouldImplementAsFunction = UEdGraphSchema_K2::CanKismetOverrideFunction(Function) && !UEdGraphSchema_K2::FunctionCanBePlacedAsEvent(Function);
		
		if(bShouldImplementAsFunction || bIsAnimFunction) {
			FName FunctionName = Function->GetFName();
			UEdGraph* FuncGraph = FindObject<UEdGraph>(Blueprint, *FunctionName.ToString());

			//If it already exists, scrap existing graph and rename it so it does not cause any conflicts
			if (FuncGraph != nullptr) {
				FBlueprintEditorUtils::RemoveGraph(Blueprint, FuncGraph, EGraphRemoveFlags::MarkTransient);
				continue;
			}

			UEdGraph* NewGraph;
			if(bIsAnimFunction) {
				NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UAnimationGraph::StaticClass(), UAnimationGraphSchema::StaticClass());
			} else {
				NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
			}
			
			NewGraph->bAllowDeletion = false;
			NewGraph->InterfaceGuid = FBlueprintEditorUtils::FindInterfaceFunctionGuid(Function, InterfaceClass);
			NewInterface.Graphs.Add(NewGraph);
			
			FBlueprintEditorUtils::AddInterfaceGraph(Blueprint, NewGraph, InterfaceClass);
		}
	}

	Blueprint->ImplementedInterfaces.Add(NewInterface);
	return true;
}

bool FBlueprintGeneratorUtils::CreateParentFunctionCall(UBlueprint* Blueprint, UFunction* Function, UK2Node* FunctionEntryOrEventNode, UK2Node_FunctionResult* FunctionReturnNode) {
	UFunction* CallableParentFunction = Blueprint->ParentClass->FindFunctionByName(Function->GetFName());

	//We can get NULL if function in question is an interface function, in that case we don't have parent function to call
	//Otherwise we should always get a valid function, since function passed should always come from one of parent classes original declaration
	if (CallableParentFunction == NULL) {
		return false;
	}

	//Spawn parent function call node and set it's function, plus allocate default pins
	UEdGraph* TargetGraph = FunctionEntryOrEventNode->GetGraph();
	const UEdGraphSchema_K2* GraphSchema = CastChecked<UEdGraphSchema_K2>(TargetGraph->GetSchema());
	
	FGraphNodeCreator<UK2Node_CallParentFunction> FunctionNodeCreator(*TargetGraph);
	
	UK2Node_CallParentFunction* ParentFunctionNode = FunctionNodeCreator.CreateNode();
	ParentFunctionNode->SetFromFunction(CallableParentFunction);
	ParentFunctionNode->AllocateDefaultPins();

	const int32 NewNodeSpawnOffset = 400;
	ParentFunctionNode->NodePosX = FunctionEntryOrEventNode->NodePosX + NewNodeSpawnOffset;
	ParentFunctionNode->NodePosY = FunctionEntryOrEventNode->NodePosY;
	FunctionNodeCreator.Finalize();
	
	//Move function result node to actually match location of newly spawned parent call node
	if (FunctionReturnNode) {
		FunctionReturnNode->NodePosX = FunctionEntryOrEventNode->NodePosX + NewNodeSpawnOffset * 2;
		FunctionReturnNode->NodePosY = FunctionEntryOrEventNode->NodePosY;
	}

	//Connect parent function pins
	for (UEdGraphPin* ParentPin : ParentFunctionNode->Pins) {
		
		//Skip all hidden pins we encounter, they will be filled by kismet compiler automatically
		if (ParentPin->bHidden) {
			continue;
		}

		//Output pins we always want to connect with the function result node
		if (ParentPin->Direction == EGPD_Output) {
		
			//Connect Then pin with the Execute pin of the function result node if we have any
			if (UEdGraphSchema_K2::IsExecPin(*ParentPin)) {
				if (FunctionReturnNode) {
					UEdGraphPin* ReturnNodeExecInputPin = GraphSchema->FindExecutionPin(*FunctionReturnNode, EGPD_Input);
					ParentPin->MakeLinkTo(ReturnNodeExecInputPin);
				}
				continue;
			}

			//Otherwise we should try to find corresponding pin on the function return node
			if (FunctionReturnNode) {
				UEdGraphPin* ReturnInputPin = FunctionReturnNode->FindPin(ParentPin->PinName, EGPD_Input);
				
				if (ReturnInputPin) {
					ReturnInputPin->BreakAllPinLinks(true);
					ReturnInputPin->MakeLinkTo(ParentPin);
				}
			}

		//Input pins we connect with function entry or event node
		} else if (ParentPin->Direction == EGPD_Input) {

			//Connect parent call exec pin with the then pin of the function entry
			if (UEdGraphSchema_K2::IsExecPin(*ParentPin)) {
				UEdGraphPin* FunctionEntryThenPin = GraphSchema->FindExecutionPin(*FunctionEntryOrEventNode, EGPD_Output);
				
				if (FunctionEntryThenPin) {
					FunctionEntryThenPin->BreakAllPinLinks(true);
					FunctionEntryThenPin->MakeLinkTo(ParentPin);
				}
				continue;
			}

			//Otherwise try to find entry pin matching our input pin name
			UEdGraphPin* FunctionEntryPin = FunctionEntryOrEventNode->FindPin(ParentPin->PinName, EGPD_Output);
			if (FunctionEntryPin) {
				FunctionEntryPin->MakeLinkTo(ParentPin);
			}
		}
	}
	return true;
}

UK2Node_Event* FBlueprintGeneratorUtils::CreateCustomEventNode(UBlueprint* Blueprint, const FName& EventName) {
	//Find all event nodes inside of the ubergraph
	TArray<UK2Node_Event*> AllEventNodes;
	for (const UEdGraph* Graph : Blueprint->UbergraphPages) {
		Graph->GetNodesOfClass(AllEventNodes);
	}

	//Make effort to find existing event node first
	for (UK2Node_Event* EventNode : AllEventNodes) {
		if (EventNode->GetFunctionName() == EventName) {
			return EventNode;
		}
	}

	//We cannot really create any events without event graph
	UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
	if (!EventGraph) {
		return NULL;
	}

	//Allocate node and set custom function name
	FGraphNodeCreator<UK2Node_CustomEvent> NodeCreator(*EventGraph);
	UK2Node_CustomEvent* CustomEventNode = NodeCreator.CreateNode();
	CustomEventNode->CustomFunctionName = EventName;
	CustomEventNode->bIsEditable = true;

	const FVector2D GoodPlaceForNode = EventGraph->GetGoodPlaceForNewNode();
	CustomEventNode->NodePosX = GoodPlaceForNode.X;
	CustomEventNode->NodePosY = GoodPlaceForNode.Y;
	
	NodeCreator.Finalize();
	return CustomEventNode;
}

UK2Node_FunctionResult* FBlueprintGeneratorUtils::CreateFunctionResultNode(const UK2Node_FunctionEntry* FunctionEntry, bool bAutoConnectNode) {
	UEdGraph* Graph = FunctionEntry->GetGraph();
	const UEdGraphSchema_K2* GraphSchema = CastChecked<UEdGraphSchema_K2>(Graph->GetSchema());
	FGraphNodeCreator<UK2Node_FunctionResult> NodeCreator(*Graph);
	
	UK2Node_FunctionResult* ReturnNode = NodeCreator.CreateNode();
	ReturnNode->FunctionReference = FunctionEntry->FunctionReference;
	ReturnNode->NodePosX = FunctionEntry->NodePosX + FunctionEntry->NodeWidth + 256;
	ReturnNode->NodePosY = FunctionEntry->NodePosY;
	NodeCreator.Finalize();

	const UFunction* FunctionSignature = FunctionEntry->FindSignatureFunction();
	ReturnNode->CreateUserDefinedPinsForFunctionEntryExit(FunctionSignature, false);

	//Auto-connect execution pins if we have been asked to
	if (bAutoConnectNode) {
		UEdGraphPin* EntryNodeExec = GraphSchema->FindExecutionPin(*FunctionEntry, EGPD_Output);
		UEdGraphPin* ResultNodeExec = GraphSchema->FindExecutionPin(*ReturnNode, EGPD_Input);

		EntryNodeExec->BreakAllPinLinks(true);
		EntryNodeExec->MakeLinkTo(ResultNodeExec);
	}
	return ReturnNode;
}

UK2Node_FunctionEntry* FBlueprintGeneratorUtils::CreateFunctionGraph(UBlueprint* Blueprint, const FName& FunctionName) {
	//If we already have a function graph with the same name, return function from it
	if (const UEdGraph* ExistingGraph = FindObject<UEdGraph>(Blueprint, *FunctionName.ToString())) {
		TArray<UK2Node_FunctionEntry*> FunctionEntries;
		ExistingGraph->GetNodesOfClass(FunctionEntries);
		
		return UE427IsEmpty(FunctionEntries) ? nullptr : FunctionEntries[0];
	}
	
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	const UEdGraphSchema_K2* GraphSchema = CastChecked<const UEdGraphSchema_K2>(NewGraph->GetSchema());

	//Create default graph nodes for a new function graph
	GraphSchema->CreateDefaultNodesForGraph(*NewGraph);
	GraphSchema->CreateFunctionGraphTerminators(*NewGraph, (UClass*) NULL);

	//Add default function flags for kismet function, since we do not inherit any flags from parent function
	int32 ExtraFunctionFlags = FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public;
	if (Blueprint->BlueprintType == BPTYPE_FunctionLibrary) {
		ExtraFunctionFlags |= FUNC_Static;
	}
	GraphSchema->AddExtraFunctionFlags(NewGraph, ExtraFunctionFlags);
	
	//Mark function entry as editable because it's a custom function and not an override
	GraphSchema->MarkFunctionEntryAsEditable(NewGraph, true);
	Blueprint->FunctionGraphs.Add(NewGraph);

	TArray<UK2Node_FunctionEntry*> FunctionEntries;
	NewGraph->GetNodesOfClass(FunctionEntries);
	return FunctionEntries[0];
}

UK2Node* FBlueprintGeneratorUtils::CreateFunctionOverride(UBlueprint* Blueprint, UFunction* Function, bool bOverrideAsEvent, bool bCreateParentCall) {
	const FName FunctionName = Function->GetFName();
	UClass* FunctionOwnerClass = Function->GetOuterUClass()->GetAuthoritativeClass();
	
	UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
	
	//Attempt to implement function as event if we can, have event graph and it's desired
	if (UEdGraphSchema_K2::FunctionCanBePlacedAsEvent(Function) && EventGraph && bOverrideAsEvent) {
	
		//Return existing event node first, in case of us having it already
		UK2Node_Event* ExistingNode = FBlueprintEditorUtils::FindOverrideForFunction(Blueprint, FunctionOwnerClass, FunctionName);

		//If it's a ghost node, we want to remove the ghost mark and create the parent call if it doesn't have one already
		if (ExistingNode != NULL && ExistingNode->IsAutomaticallyPlacedGhostNode()) {
			ResetNodeDisabledState(ExistingNode);
			
			UEdGraphPin* ExecPin = ExistingNode->FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
			if (ExecPin == NULL) {
				return ExistingNode;
			}
			
			if (ExecPin->HasAnyConnections()) {
				//If we have connections, we want to determine all of the connected nodes and make sure none of them is marked as disabled
				for (const UEdGraphPin* ConnectedPin : ExecPin->LinkedTo) {
					UEdGraphNode* OwningNode = ConnectedPin->GetOwningNode();
						
					if (OwningNode->IsAutomaticallyPlacedGhostNode()) {
						ResetNodeDisabledState(OwningNode);
					}
				}
			} else {
				//If exec pin is not connected to anything, we actually want to create the parent call node normally
				if (bCreateParentCall) {
					CreateParentFunctionCall(Blueprint, Function, ExistingNode, NULL);
				}
			}
		}

		//If node exists and it's not a ghost node, we just return early
		if (ExistingNode) {
			return ExistingNode;
		}
		
		UK2Node_Event* Event = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_Event>(EventGraph, EventGraph->GetGoodPlaceForNewNode(), EK2NewNodeFlags::None,
			[&](UK2Node_Event* NewInstance){
        	NewInstance->EventReference.SetExternalMember(FunctionName, FunctionOwnerClass);
        	NewInstance->bOverrideFunction = true;
        });

		if (bCreateParentCall) {
			CreateParentFunctionCall(Blueprint, Function, Event, NULL);
		}
		return Event;
	}

	//Otherwise create function override using separate graph
	//Returning existing function node if we already have an override
	if (const UEdGraph* ExistingGraph = FindObject<UEdGraph>(Blueprint, *FunctionName.ToString())) {
		TArray<UK2Node_FunctionEntry*> FunctionEntries;
		ExistingGraph->GetNodesOfClass(FunctionEntries);
		
		return FunctionEntries[0];
	}
		
	//Implement the function graph and spawn default nodes
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	const UEdGraphSchema_K2* GraphSchema = CastChecked<const UEdGraphSchema_K2>(NewGraph->GetSchema());
	
	GraphSchema->CreateDefaultNodesForGraph(*NewGraph);
	GraphSchema->CreateFunctionGraphTerminators(*NewGraph, FunctionOwnerClass);
	
	Blueprint->FunctionGraphs.Add(NewGraph);

	UK2Node_FunctionEntry* FunctionEntry = NULL;
	UK2Node_FunctionResult* FunctionResult = NULL;

	//Find function entry and result nodes
	for (UEdGraphNode* GraphNode : NewGraph->Nodes) {
		if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(GraphNode)) {
			FunctionEntry = Entry;
		}
		if (UK2Node_FunctionResult* Result = Cast<UK2Node_FunctionResult>(GraphNode)) {
			FunctionResult = Result;
		}
	}
	checkf(FunctionEntry, TEXT("K2Node_FunctionEntry not found on newly created function graph"));

	//Make parent function call if we've been asked to
	if (bCreateParentCall) {
		CreateParentFunctionCall(Blueprint, Function, FunctionEntry, FunctionResult);
	}
	return FunctionEntry;
}

FName FBlueprintGeneratorUtils::GetFunctionNameForNode(UK2Node* Node) {
	if (const UK2Node_Event* Event = Cast<UK2Node_Event>(Node)) {
		return Event->GetFunctionName();
	}
	if (const UK2Node_FunctionEntry* FunctionEntry = Cast<UK2Node_FunctionEntry>(Node)) {
		return FunctionEntry->CustomGeneratedFunctionName != NAME_None ? FunctionEntry->CustomGeneratedFunctionName : FunctionEntry->GetGraph()->GetFName();
	}
	return NAME_None;
}

UFunction* FBlueprintGeneratorUtils::FindFunctionInParentClassOrInterfaces(UBlueprint* Blueprint, const FName& FunctionName) {
	//Attempt to find function inside of the implemented interfaces first
	UFunction* ResultFunction = FBlueprintEditorUtils::GetInterfaceFunction(Blueprint, FunctionName);

	//Search up the class hierarchy, we want to find the original declaration of the function
	//because that way we do not depend on overrides being removed or added, and also match the behavior of original BP editor
	if (ResultFunction == NULL) {
		const UClass* ParentClass = Blueprint->ParentClass;

		const UClass* CurrentClass = ParentClass->GetSuperClass();
		ResultFunction = ParentClass->FindFunctionByName(FunctionName);
		
		while (CurrentClass != NULL && ResultFunction != NULL) {
			if (UFunction* ParentFunction = CurrentClass->FindFunctionByName(FunctionName)) {
				ResultFunction = ParentFunction;
			} else {
				break;
			}
			CurrentClass = CurrentClass->GetSuperClass();
		}
	}
	return ResultFunction;
}


bool FBlueprintGeneratorUtils::CreateNewBlueprintFunctions(UBlueprint* Blueprint, const TArray<FDeserializedFunction>& Functions, const TFunctionRef<bool(const FDeserializedFunction& Function)>& FunctionFilter, bool bCreateFunctionOverrides) {
	TSet<FName> ObsoleteBlueprintFunctions;
	bool bChangedFunctionsOrParameters = false;

	//Map all existing functions to their names, taking into account both function overrides and events
	TMap<FName, UK2Node_EditablePinBase*> FunctionAndEventNodes;

	TArray<UEdGraph*> AllBlueprintGraphs;
	Blueprint->GetAllGraphs(AllBlueprintGraphs);

	for (UEdGraph* Graph : AllBlueprintGraphs) {
		for (UEdGraphNode* GraphNode : Graph->Nodes) {
			if (UK2Node_Event* Event = Cast<UK2Node_Event>(GraphNode)) {
				FunctionAndEventNodes.Add(Event->GetFunctionName(), Event);
			}
			if (UK2Node_FunctionEntry* FunctionEntry = Cast<UK2Node_FunctionEntry>(GraphNode)) {
				FunctionAndEventNodes.Add(GetFunctionNameForNode(FunctionEntry), FunctionEntry);
			}
		}
	}

	//Populate a list of existing functions and events in the blueprint
	FunctionAndEventNodes.GetKeys(ObsoleteBlueprintFunctions);
	
	//Create new functions and make sure signature of existing ones matches
	for (const FDeserializedFunction& Function : Functions) {
		const FString FunctionName = Function.FunctionName.ToString();
		if (FunctionName.EndsWith(TEXT("__UpdateFunc")) || FunctionName.EndsWith(TEXT("__FinishedFunc"))) continue;
		ObsoleteBlueprintFunctions.Remove(Function.FunctionName);

		//Discard functions that are generated, e.g. ubergraph
		if (IsFunctionNameGenerated(Function.FunctionName.ToString())) {
			continue;
		}
		//Discard functions that don't pass property filter
		if (!FunctionFilter(Function)) {
			continue;
		}

		//Determine whenever we're dealing with function override or new function
		UFunction* OverridenFunction = FindFunctionInParentClassOrInterfaces(Blueprint, Function.FunctionName);

		//If we're overriding a function, then we do not actually need to do anything except than checking that override exists
		if (OverridenFunction) {
			UK2Node_EditablePinBase* const* FunctionImplementation = FunctionAndEventNodes.Find(Function.FunctionName);

			//We want to create the override even if function exists, but it's actually a ghost node
			if ((FunctionImplementation == NULL || (*FunctionImplementation)->IsAutomaticallyPlacedGhostNode()) && bCreateFunctionOverrides) {
				const bool bShouldGenerateAsEvent = Function.bIsCallingIntoUbergraph && UEdGraphSchema_K2::FunctionCanBePlacedAsEvent(OverridenFunction);
				
				CreateFunctionOverride(Blueprint, OverridenFunction, bShouldGenerateAsEvent, true);
				bChangedFunctionsOrParameters = true;
			}
			continue;
		}
		
		//Try to find existing function entry or event first
		UK2Node_EditablePinBase* FunctionEntryOrEventNode;
		TArray<UK2Node_FunctionResult*> FunctionResultNodes;

		//Try to find existing function or event node first
		if (UK2Node_EditablePinBase* const* ExistingNode = FunctionAndEventNodes.Find(Function.FunctionName)) {
			FunctionEntryOrEventNode = *ExistingNode;

			//Retrieve function result nodes if we're dealing with function graph
			if (const UK2Node_FunctionEntry* FunctionEntry = Cast<const UK2Node_FunctionEntry>(FunctionEntryOrEventNode)) {
				FunctionEntry->GetGraph()->GetNodesOfClass(FunctionResultNodes);
			}
		} else {
			const bool bShouldImplementAsEvent = Function.bIsCallingIntoUbergraph;
			
			if (!bShouldImplementAsEvent) {
				//If we should create new function as an actual function graph, make sure to also create function result node
				UK2Node_FunctionEntry* FunctionEntry = CreateFunctionGraph(Blueprint, Function.FunctionName);
				if (!FunctionEntry) continue;
				FunctionEntryOrEventNode = FunctionEntry;
				
				//Create function result node if function has any output parameters
				if (Function.HasAnyOutputParams()) {
					UK2Node_FunctionResult* FunctionResult = CreateFunctionResultNode(FunctionEntry, true);
					FunctionResultNodes.Add(FunctionResult);
				}
			} else {
				//Implement function as event, event functions cannot really have function return nodes
				FunctionEntryOrEventNode = CreateCustomEventNode(Blueprint, Function.FunctionName);
			}
			bChangedFunctionsOrParameters = true;
		}

		//Make sure we have all flags from the deserialized function
		if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(FunctionEntryOrEventNode)) {
			if (EventNode->FunctionFlags != Function.FunctionFlags) {
				
				EventNode->FunctionFlags = (Function.FunctionFlags & ~FUNC_Native);
				bChangedFunctionsOrParameters = true;
			}
		} else if (UK2Node_FunctionEntry* FunctionEntryNode = Cast<UK2Node_FunctionEntry>(FunctionEntryOrEventNode)) {
			if (FunctionEntryNode->GetExtraFlags() != Function.FunctionFlags) {

				FunctionEntryNode->SetExtraFlags(Function.FunctionFlags);
				bChangedFunctionsOrParameters = true;
			}
		}

		//Make sure parameters on the function declaration stay up to date
		bChangedFunctionsOrParameters |= SetFunctionEntryParameters(FunctionEntryOrEventNode, Function, true);

		//Update parameters on function result nodes if we have any
		for (UK2Node_FunctionResult* ResultNode : FunctionResultNodes) {
			bChangedFunctionsOrParameters |= SetFunctionEntryParameters(ResultNode, Function, false);
		}
	}

	//Remove functions we actually used to implement in the past but no longer do
	for (const FName& ObsoleteFunctionName : ObsoleteBlueprintFunctions) {
		UK2Node* ExistingFunctionNode = FunctionAndEventNodes.FindChecked(ObsoleteFunctionName);
		UEdGraph* NodeGraph = ExistingFunctionNode->GetGraph();
		if (Blueprint->DelegateSignatureGraphs.Contains(NodeGraph)) continue;

		//We can remove event nodes as long as they're located inside of the ubergraph
		if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(ExistingFunctionNode)) {
			if (Blueprint->UbergraphPages.Contains(NodeGraph)) {
				
				FBlueprintEditorUtils::RemoveNode(Blueprint, EventNode, true);
				bChangedFunctionsOrParameters = true;
				continue;
			}
		}

		//For function nodes, we need to remove entire graphs, but make sure they're function graphs beforehand
		if (Cast<UK2Node_FunctionEntry>(ExistingFunctionNode)) {
			if (Blueprint->FunctionGraphs.Contains(NodeGraph)) {
				
				FBlueprintEditorUtils::RemoveGraph(Blueprint, NodeGraph, EGraphRemoveFlags::MarkTransient);
				bChangedFunctionsOrParameters = true;
				continue;
			}
		}

		//Log nodes we couldn't really delete because they are not located in any known places
		UE_LOG(LogAssetGenerator, Error, TEXT("Couldn't delete obsolete function %s inside of the graph %s because Graph is not an ubergraph or function graph"),
			*ObsoleteFunctionName.ToString(), *NodeGraph->GetPathName());
	}

	return bChangedFunctionsOrParameters;
}

bool FBlueprintGeneratorUtils::CreateBlueprintVariablesForProperties(UBlueprint* Blueprint, const TArray<FDeserializedProperty>& Properties,
                                                                     const TMap<FName, FDeserializedFunction>& Functions, const TFunctionRef<bool(const FDeserializedProperty& Property)>& PropertyFilter) {
	TSet<FName> ObsoleteBlueprintProperties;
	bool bChangedProperties = false;

	//Populate a list of all currently existing variables in the blueprint
	for (const FBPVariableDescription& Desc : Blueprint->NewVariables) {
		ObsoleteBlueprintProperties.Add(Desc.VarName);
	}
	
	//Create new variables and update existing ones
	for (const FDeserializedProperty& Property : Properties) {
		ObsoleteBlueprintProperties.Remove(Property.PropertyName);

		//Discard properties that are 100% kismet compiler generated
		if (IsPropertyNameGenerated(Property.PropertyName.ToString())) {
			continue;
		}
		//Discard variables that didn't pass the provided property filter
		if (!PropertyFilter(Property)) {
			continue;
		}
		
		//Try to find existing variable using it's name
		FBPVariableDescription* VariableDescription = FindBlueprintVariableByName(Blueprint, Property.PropertyName);

		//Create new variable if we didn't find existing one
		if (VariableDescription == NULL) {
			VariableDescription = &Blueprint->NewVariables.AddDefaulted_GetRef();

			VariableDescription->VarName = Property.PropertyName;
			VariableDescription->VarGuid = FGuid::NewGuid();
			VariableDescription->FriendlyName = Property.PropertyName.ToString();
			
			VariableDescription->Category = UEdGraphSchema_K2::VR_DefaultCategory;
			VariableDescription->SetMetaData(TEXT("MultiLine"), TEXT("true"));
			bChangedProperties = true;
		} 

		//Apply data to the variable description
		if (VariableDescription->VarType != Property.GraphPinType) {
			VariableDescription->VarType = Property.GraphPinType;
			bChangedProperties = true;
		}
		
		if (VariableDescription->PropertyFlags != (uint64) Property.PropertyFlags) {
			VariableDescription->PropertyFlags = (uint64) Property.PropertyFlags;
			bChangedProperties = true;
		}
		
		if (VariableDescription->RepNotifyFunc != Property.RepNotifyFunc) {
			VariableDescription->RepNotifyFunc = Property.RepNotifyFunc;
			bChangedProperties = true;
		}
		
		if (VariableDescription->ReplicationCondition != Property.BlueprintReplicationCondition) {
			VariableDescription->ReplicationCondition = Property.BlueprintReplicationCondition;
			bChangedProperties = true;
		}
		
		//Make sure multicast delegates actually have valid graphs
		if (VariableDescription->VarType.PinCategory == UEdGraphSchema_K2::PC_MCDelegate) {
			UEdGraph* DelegateSignatureGraph = FBlueprintEditorUtils::GetDelegateSignatureGraphByName(Blueprint, VariableDescription->VarName);
			
			if (DelegateSignatureGraph == NULL) {
				DelegateSignatureGraph = CreateNewBlueprintEventDispatcherSignatureGraph(Blueprint, VariableDescription->VarName);
			}

			//Make sure delegate function parameters are up to date
			TArray<UK2Node_FunctionEntry*> FunctionEntries;
			DelegateSignatureGraph->GetNodesOfClass(FunctionEntries);
			check(FunctionEntries.Num());

			const FString DelegateSignatureFunctionName = FString::Printf(TEXT("%s%s"), *VariableDescription->VarName.ToString(), HEADER_GENERATED_DELEGATE_SIGNATURE_SUFFIX);
			// TODO: In AnimBPs this map can be empty (as functions aren't generated into the JSON yet) so causes a crash - is this a proper fix?
			if (Functions.Num() != 0) {
				const FDeserializedFunction& DelegateSignature = Functions.FindChecked(*DelegateSignatureFunctionName);
			
				if (SetFunctionEntryParameters(FunctionEntries[0], DelegateSignature, true)) {
					bChangedProperties = true;
				}	
			}
		}
	}

	if (ObsoleteBlueprintProperties.Num()) {
		bChangedProperties = true;
	}

	//Remove delegate signature graphs for obsolete properties
	TArray<UEdGraph*> ObsoleteDelegateSignatureGraphs;
	
	for (UEdGraph* DelegateSignatureGraph : Blueprint->DelegateSignatureGraphs) {
		if (ObsoleteBlueprintProperties.Contains(DelegateSignatureGraph->GetFName())) {
			ObsoleteDelegateSignatureGraphs.Add(DelegateSignatureGraph);
		}
	}
	for (UEdGraph* DelegateSignatureGraph : ObsoleteDelegateSignatureGraphs) {
		FBlueprintEditorUtils::RemoveGraph(Blueprint, DelegateSignatureGraph, EGraphRemoveFlags::MarkTransient);
	}
	
	//Remove obsolete variables descriptors
	Blueprint->NewVariables.RemoveAll([&](const FBPVariableDescription& Desc){
		return ObsoleteBlueprintProperties.Contains(Desc.VarName);
	});
	return bChangedProperties;
}

bool FBlueprintGeneratorUtils::SetFunctionEntryParameters(UK2Node_EditablePinBase* FunctionEntry, const FDeserializedFunction& Function, bool bIsFunctionEntry) {
	TArray<FDeserializedProperty> ParameterArray;

	//Collect all parameters matching our input parameter boolean (because we can be dealing with function result node instead)
	for (const FDeserializedProperty& Parameter : Function.Parameters) {
		if (Parameter.IsFunctionInputParam() == bIsFunctionEntry) {
			ParameterArray.Add(Parameter);
		}
	}
	
	const TArray<TSharedPtr<FUserPinInfo>> OldUserDefinedPins = FunctionEntry->UserDefinedPins;
	bool bNeedToDeleteExistingPins = false;
	
	for (int32 i = 0; i < OldUserDefinedPins.Num(); i++) {
		//Cleanup properties if there are more of them than we expect
		if (!ParameterArray.IsValidIndex(i)) {
			bNeedToDeleteExistingPins = true;
			break;
		}

		const TSharedPtr<FUserPinInfo>& UserPinInfo = OldUserDefinedPins[i];
		const FDeserializedProperty& Property = ParameterArray[i];

		//Cleanup properties if names or pin types of one of them do not match
		if (UserPinInfo->PinName != Property.PropertyName ||
			UserPinInfo->PinType != Property.GraphPinType) {
			bNeedToDeleteExistingPins = true;
			break;
		}

		//TEMPFIX: If current user pins have the invalid __WorldContext pin generated, we need to regenerate pins
		if (UserPinInfo->PinName == TEXT("__WorldContext")) {
			bNeedToDeleteExistingPins = true;
			break;
		}
	}
	
	//Cleanup pins if we need to because they overlap with new ones
	if (bNeedToDeleteExistingPins) {
		for (const TSharedPtr<FUserPinInfo>& UserPinInfo : OldUserDefinedPins) {
			FunctionEntry->RemoveUserDefinedPin(UserPinInfo);
		}
	}

	//Add new pins, but skip over the ones we already have except if we cleaned them up
	const int32 FirstPinIndex = bNeedToDeleteExistingPins ? 0 : OldUserDefinedPins.Num();
	
	for (int32 i = FirstPinIndex; i < ParameterArray.Num(); i++) {
		const FDeserializedProperty& Property = ParameterArray[i];

		//Skip the pin if it represents a compiler-generated world context pin
		if (Property.PropertyName != TEXT("__WorldContext")) {
			FunctionEntry->CreateUserDefinedPin(Property.PropertyName, Property.GraphPinType, bIsFunctionEntry ? EGPD_Output : EGPD_Input, false);
		}
	}

	//We have changed something as long as first pin index is not the last one
	return FirstPinIndex != ParameterArray.Num();
}

UEdGraph* FBlueprintGeneratorUtils::CreateNewBlueprintEventDispatcherSignatureGraph(UBlueprint* Blueprint, const FName& DispatcherName) {
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, DispatcherName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	checkf(NewGraph, TEXT("Failed to create new graph %s for Event Dispatcher in BLueprint %s"), *DispatcherName.ToString(), *Blueprint->GetPathName());
	
	NewGraph->bEditable = false;
	K2Schema->CreateDefaultNodesForGraph(*NewGraph);
	K2Schema->CreateFunctionGraphTerminators(*NewGraph, (UClass*) NULL);
	
	K2Schema->AddExtraFunctionFlags(NewGraph, FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public);
	K2Schema->MarkFunctionEntryAsEditable(NewGraph, true);

	Blueprint->DelegateSignatureGraphs.Add(NewGraph);
	return NewGraph;
}

FBPVariableDescription* FBlueprintGeneratorUtils::FindBlueprintVariableByName(UBlueprint* Blueprint, const FName& VariableName) {
	for (FBPVariableDescription& VariableDescription : Blueprint->NewVariables) {
		if (VariableDescription.VarName == VariableName) {
			return &VariableDescription;
		}
	}
	return NULL;
}

void FBlueprintGeneratorUtils::ResetNodeDisabledState(UEdGraphNode* GraphNode) {
	GraphNode->SetEnabledState(ENodeEnabledState::Enabled, true);
	GraphNode->bCommentBubblePinned = false;
	GraphNode->bCommentBubbleVisible = false;
	GraphNode->NodeComment = TEXT("");
}

#undef LOCTEXT_NAMESPACE
