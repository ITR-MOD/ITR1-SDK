#include "NpcPointQueryParam.h"

FNpcPointQueryParam::FNpcPointQueryParam() {
    this->NpcCharacterRef = NULL;
    this->SearchRadius = 0.00f;
    this->MinDistanceToCenter = 0.00f;
    this->bDoPathLengthCheck = false;
    this->MaxPathLength = 0.00f;
    this->bDoSquadDistanceCheck = false;
    this->MinSquadDistance = 0.00f;
    this->MaxSearchIteration = 0;
    this->bAlwaysMoveTowardsDesiredLocation = false;
    this->bUseDistantPointAsQueryCenter = false;
    this->bForceFastPointSearch = false;
}

