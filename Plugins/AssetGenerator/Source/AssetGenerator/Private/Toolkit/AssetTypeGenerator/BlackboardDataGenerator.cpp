#include "Toolkit/AssetTypeGenerator/BlackboardDataGenerator.h"
#include "UE427AssetPath.h"
#include "BehaviorTree/BlackboardData.h"

UClass* UBlackboardDataGenerator::GetAssetObjectClass() const {
	return UBlackboardData::StaticClass();
}

FTopLevelAssetPath UBlackboardDataGenerator::GetAssetClass() {
	return FTopLevelAssetPath(UBlackboardData::StaticClass());
}
