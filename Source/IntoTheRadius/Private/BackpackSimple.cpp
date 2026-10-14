#include "BackpackSimple.h"

ABackpackSimple::ABackpackSimple(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	PrimaryActorTick.bStartWithTickEnabled = true;
	void RegisterActorTickFunctions
	(
		bool bRegister
	);
	AActor::SetActorTickEnabled(true);
}







TArray<APRHolder*> ABackpackSimple::GetHolders() {
    return TArray<APRHolder*>();
}





