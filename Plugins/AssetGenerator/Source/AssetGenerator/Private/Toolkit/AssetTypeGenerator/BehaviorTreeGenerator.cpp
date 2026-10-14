#include "Toolkit/AssetTypeGenerator/BehaviorTreeGenerator.h"
#include "UE427AssetPath.h"
#include "BehaviorTree/BehaviorTree.h"

UClass* UBehaviorTreeGenerator::GetAssetObjectClass() const {
	return UBehaviorTree::StaticClass();
}

FTopLevelAssetPath UBehaviorTreeGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UBehaviorTree::StaticClass());
}
