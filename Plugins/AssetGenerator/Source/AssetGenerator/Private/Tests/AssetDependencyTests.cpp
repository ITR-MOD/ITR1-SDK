#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Toolkit/AssetGeneration/AssetGenerationProcessor.h"
#include "UE427AssetPath.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

struct FAssetDependencyTestAccess {
	static bool Run(FAutomationTestBase& Test) {
		FAssetGeneratorConfiguration Configuration;
		Configuration.bTickOnTheSide = true;
		FAssetGenerationProcessor Processor(Configuration, {});
		auto Add = [&Processor](const FString& Name) {
			auto* Generator = NewObject<UAssetTypeGenerator>();
			Generator->PackageName = FName(*Name);
			Generator->AddToRoot();
			Processor.AssetGenerators.Add(Generator->PackageName, Generator);
			Processor.Statistics.TotalAssetPackages++;
			return Generator;
		};
		auto Link = [&Processor](UAssetTypeGenerator* Parent, UAssetTypeGenerator* Dependency) {
			auto List = MakeShared<FDependencyList>();
			List->AssetTypeGenerator = Parent;
			List->PackageDependencies.Add(Dependency->PackageName, EAssetGenerationStage::CONSTRUCTION);
			Processor.PendingDependencies.FindOrAdd(Dependency->PackageName).Add(List);
		};
		auto* Independent = Add(TEXT("/Game/DependencyCheck/Independent"));
		Processor.GeneratorsReadyToAdvance.Add(Independent);
		UAssetTypeGenerator* Previous = nullptr;
		for (int32 Index = 0; Index < 1024; ++Index) {
			auto* Current = Add(FString::Printf(TEXT("/Game/DependencyCheck/Chain%d"), Index));
			if (Previous) Link(Previous, Current);
			Previous = Current;
		}
		Processor.FailPackage(Previous->PackageName, TEXT("Missing leaf dependency"));
		Test.TestEqual(TEXT("Failure propagates through the entire chain"), Processor.Statistics.AssetPackagesSkipped, 1024);
		Test.TestEqual(TEXT("Independent asset remains scheduled"), Processor.GeneratorsReadyToAdvance.Num(), 1);
		Test.TestTrue(TEXT("Independent asset remains rooted"), Independent->IsRooted());
		Test.TestTrue(TEXT("Failed wait entries are removed"), UE427IsEmpty(Processor.PendingDependencies));
		Test.TestTrue(TEXT("Failed assets are not marked generated"), UE427IsEmpty(Processor.AlreadyGeneratedPackages));
		Processor.FailPackage(Previous->PackageName, TEXT("Repeated failure"));
		Test.TestEqual(TEXT("Repeated failure does not double-count"), Processor.Statistics.AssetPackagesSkipped, 1024);
		Independent->RemoveFromRoot();
		Processor.AssetGenerators.Reset();
		Processor.GeneratorsReadyToAdvance.Reset();

		auto* First = Add(TEXT("/Game/DependencyCheck/CycleA"));
		auto* Second = Add(TEXT("/Game/DependencyCheck/CycleB"));
		Link(First, Second);
		Link(Second, First);
		int32 Advanced = 0;
		Processor.TickAssetGeneration(Advanced);
		Test.TestTrue(TEXT("Cycle is removed without advancing either asset"), UE427IsEmpty(Processor.AssetGenerators) && Advanced == 0);
		Test.TestTrue(TEXT("Cycle leaves no stale waiters"), UE427IsEmpty(Processor.PendingDependencies));
		Test.TestFalse(TEXT("Cycle asset is released"), First->IsRooted());

		auto* Parent = Add(TEXT("/Game/DependencyCheck/Parent"));
		auto* Dependency = Add(TEXT("/Game/DependencyCheck/Dependency"));
		Link(Parent, Dependency);
		Dependency->CurrentStage = EAssetGenerationStage::DATA_POPULATION;
		Processor.OnGeneratorStageAdvanced(Dependency);
		Test.TestTrue(TEXT("Parent becomes ready after the required dependency stage"), Processor.GeneratorsReadyToAdvance.Contains(Parent));
		Test.TestTrue(TEXT("Satisfied dependency is removed"), UE427IsEmpty(Processor.PendingDependencies));
		for (const auto& Entry : Processor.AssetGenerators) Entry.Value->RemoveFromRoot();
		Processor.AssetGenerators.Reset();
		Processor.GeneratorsReadyToAdvance.Reset();

		auto* Validator = NewObject<UAssetTypeGenerator>();
		auto Package = MakeShared<FJsonObject>();
		Package->SetStringField(TEXT("Type"), TEXT("Import"));
		Package->SetStringField(TEXT("ObjectName"), TEXT("/Script/CoreUObject"));
		auto Object = MakeShared<FJsonObject>();
		Object->SetStringField(TEXT("Type"), TEXT("Import"));
		Object->SetStringField(TEXT("ClassPackage"), TEXT("/Script/CoreUObject"));
		Object->SetStringField(TEXT("ClassName"), TEXT("Class"));
		Object->SetStringField(TEXT("ObjectName"), TEXT("MissingDependencyCheckClass"));
		Object->SetNumberField(TEXT("Outer"), 0);
		Validator->DependencyObjects.Add(0, Package);
		Validator->DependencyObjects.Add(1, Object);
		TSet<FName> Required { FName(TEXT("/Script/CoreUObject")) };
		FString Reason;
		Test.TestFalse(TEXT("Existing package with missing object is rejected"), Validator->ValidateDependencyObjects(Required, Reason));
		Test.TestTrue(TEXT("Failure identifies the missing object"), Reason.Contains(TEXT("MissingDependencyCheckClass")));
		Object->SetStringField(TEXT("ObjectName"), TEXT("Object"));
		Test.TestTrue(TEXT("Existing dependency object is accepted"), Validator->ValidateDependencyObjects(Required, Reason));
		return true;
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAssetDependencyHandlingTest, "AssetGenerator.Dependencies.SchedulingAndFailures", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAssetDependencyHandlingTest::RunTest(const FString& Parameters) {
	return FAssetDependencyTestAccess::Run(*this);
}

#endif

#endif
