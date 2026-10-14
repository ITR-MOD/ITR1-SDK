#include "CoverBTTask.h"

UCoverBTTask::UCoverBTTask() {
    this->MaxSearchIteration = 1;
    this->SearchRadius = 4000.00f;
    this->MinDistanceToTarget = 100.00f;
    this->bDoPathLengthCheck = true;
    this->MaxPathLength = 1000.00f;
    this->bDoSquadDistanceCheck = true;
    this->MinSquadDistance = 2000.00f;
    this->bAlwaysMoveTowardsDesiredLocation = false;
    this->bUseDistantPointAsQueryCenter = false;
    this->bForceFastPointSearch = false;
    this->BehaviorTreeComponent = NULL;
}

void UCoverBTTask::OnQueryComplete(FNpcPointQueryResult Result) {
}


