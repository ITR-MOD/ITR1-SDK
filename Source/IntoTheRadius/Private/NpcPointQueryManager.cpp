#include "NpcPointQueryManager.h"

UNpcPointQueryManager::UNpcPointQueryManager() {
}

UNpcPointQueryManager* UNpcPointQueryManager::GetNpcPointQueryManager() {
    return NULL;
}

FNpcPointQueryHandle UNpcPointQueryManager::FindShootingPoint(const FNpcPointQueryParam& Param, FPointQueryCompleteDelegate FinishCallback, ENpcPointQueryType QueryType) {
    return FNpcPointQueryHandle{};
}

FNpcPointQueryHandle UNpcPointQueryManager::FindCoverPoint(const FNpcPointQueryParam& Param, FPointQueryCompleteDelegate FinishCallback) {
    return FNpcPointQueryHandle{};
}

void UNpcPointQueryManager::CancelQuery(FNpcPointQueryHandle& Handle) {
}


