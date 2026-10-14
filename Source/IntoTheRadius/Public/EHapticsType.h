#pragma once
#include "CoreMinimal.h"
#include "EHapticsType.generated.h"

UENUM(BlueprintType)
enum class EHapticsType : uint8 {
    None,
    Death,
    DistanceGrip_Left,
    DistanceGrip_Right,
    BulletHit = 6,
    Climb_Left,
    Climb_Right,
    Ping_Left,
    Ping_Right,
    Rumble,
    Heartbeat,
    BackpackTake,
    BackpackReturn,
    Eat,
    Drink,
    Smoke,
    SitDown,
    Inject,
    Inject_Left,
    Inject_Right,
    Ping_Chest_Left,
    Ping_Chest_Right,
    KnifeHit_Left,
    KnifeHit_Right,
    DamageFall_Light = 31,
    DamageFall_Heavy,
    DamageSlash_Front_Left,
    DamageSlash_Back_Left,
    DamageSlash_Front_Right,
    DamageSlash_Back_Right,
    DamageZone_Light,
    DamageZone_Heavy,
    Recoil_Left = 40,
    Recoil_Right,
    Level_Transfer,
};

