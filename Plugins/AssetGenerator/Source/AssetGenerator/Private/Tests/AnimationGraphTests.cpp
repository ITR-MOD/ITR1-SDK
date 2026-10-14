#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Misc/AutomationTest.h"
#include "UE427AssetPath.h"
#include "GraphReconstruction/AnimationGraphReconstruction.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Serialization/JsonSerializer.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASAnimationGraphTest, "AssetGenerator.Graphs.AnimationBlueprint", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASAnimationGraphTest::RunTest(const FString& Parameters)
{
    auto Blueprint = CastChecked<UAnimBlueprint>(FKismetEditorUtilities::CreateBlueprint(UAnimInstance::StaticClass(), GetTransientPackage(), MakeUniqueObjectName(GetTransientPackage(), UAnimBlueprint::StaticClass(), TEXT("CASAnimationTest")), BPTYPE_Normal, UAnimBlueprint::StaticClass(), UAnimBlueprintGeneratedClass::StaticClass()));
    Blueprint->bIsTemplate = true;
    FEdGraphPinType Sequence;
    Sequence.PinCategory = UEdGraphSchema_K2::PC_Object;
    Sequence.PinSubCategoryObject = UAnimSequenceBase::StaticClass();
    FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("Animation"), Sequence);
    FEdGraphPinType Time;
    Time.PinCategory = UEdGraphSchema_K2::PC_Float;
    Time.PinSubCategory = UEdGraphSchema_K2::PC_Float;
    FBlueprintEditorUtils::AddMemberVariable(Blueprint, TEXT("Time"), Time, TEXT("0.25"));
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
    TSharedPtr<FJsonObject> Data;
    FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(TEXT(R"({"Version":1,"Nodes":[{"Name":"Root","Type":"AnimNode_Root","Properties":{"Result":{"LinkID":1}},"Bindings":{}},{"Name":"Evaluator","Type":"AnimNode_SequenceEvaluator","Properties":{"bShouldLoop":true,"bTeleportToExplicitTime":true},"Bindings":{"Sequence":["Animation"],"ExplicitTime":["Time"]}}]})")), Data);
    FString Error;
    if (!TestTrue(TEXT("Build recovered evaluator graph"), AnimationGraphReconstruction::Build(Blueprint, Data, Error))) { AddError(Error); return false; }
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
    TestTrue(TEXT("Template compiles"), Blueprint->Status != BS_Error);
    const auto Generated = CastChecked<UAnimBlueprintGeneratedClass>(Blueprint->GeneratedClass);
    TestEqual(TEXT("Two compiled animation nodes"), Generated->GetAnimNodeProperties().Num(), 2);
    TestNotNull(TEXT("Compiler generated sparse animation constants"), Generated->GetSparseClassDataStruct());
    const int32 GraphCount = Blueprint->FunctionGraphs.Num();
    TestTrue(TEXT("Rebuilding supported graph succeeds"), AnimationGraphReconstruction::Build(Blueprint, Data, Error));
    TestEqual(TEXT("Rebuild does not duplicate graphs"), Blueprint->FunctionGraphs.Num(), GraphCount);
    Data->GetArrayField(TEXT("Nodes"))[1]->AsObject()->SetStringField(TEXT("Type"), TEXT("AnimNode_StateMachine"));
    TestFalse(TEXT("Unsupported node is rejected before editing"), AnimationGraphReconstruction::Build(Blueprint, Data, Error));
    TestEqual(TEXT("Rejected graph retains existing nodes"), Blueprint->FunctionGraphs.Num(), GraphCount);
    return true;
}
#endif

#endif
