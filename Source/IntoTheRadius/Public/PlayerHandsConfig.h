#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "EPRControllerType.h"
#include "EPRHandPoseGeometryType.h"
#include "EPRHandPoseNumber.h"
#include "EPRHandPoseType.h"
#include "IAsset.h"
#include "IPreparable.h"
#include "PRCleanTransform.h"
#include "PRHandPoseConditions.h"
#include "PRHandPoseInfo.h"
#include "PRHandPoseInfosPair.h"
#include "ShiftsPair.h"
#include "PlayerHandsConfig.generated.h"

class UDataTable;
class UGripMotionControllerComponent;
class UObject;
class UPlayerHandsConfig;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMesh;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UPlayerHandsConfig : public UDataAsset, public IIAsset, public IIPreparable {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FShiftsPair MotionSourcesDefaultPosition;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    TMap<EPRControllerType, FShiftsPair> ControllerShifts;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    float PistolsStickBreakDistance;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* PrimaryHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* PrimaryTriggeredHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* PrimarySafetyHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* SecondaryHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* ThrowHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    UDataTable* SlideHandPosesTable;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    FPRHandPoseInfosPair HandsPoseDefault;
    
    UPlayerHandsConfig();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTransform ToTransform(FPRCleanTransform cleanTransform);
    
    UFUNCTION(BlueprintCallable)
    static void ShowDeviceModel(USceneComponent* Parent, UPrimitiveComponent* DisplayComponent, EControllerHand hand, UPrimitiveComponent*& newDisplayComponent);
    
    UFUNCTION(BlueprintCallable)
    void SaveToAsset();
    
    UFUNCTION(BlueprintCallable)
    void SavePoseDefault(TMap<EPRControllerType, FPRCleanTransform> leftItemShifts, TMap<EPRControllerType, FPRCleanTransform> rightItemShifts, FPRHandPoseInfosPair handsData);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForTagByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, FGameplayTag Tag, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForTag(const EPRHandPoseType PoseType, const FGameplayTag Tag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForStaticMeshByGeometryTypeAndPoseNumber(const EPRHandPoseType PoseType, UStaticMesh* StaticMesh, const EPRHandPoseGeometryType GeometryType, const EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForStaticMesh(const EPRHandPoseType PoseType, UStaticMesh* StaticMesh) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UObject* Object, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HaveHandsPoseForObject(EPRHandPoseType PoseType, UObject* Object) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FTransform GetShiftForController(const FPRHandPoseInfo& Info, const EPRControllerType& Controller);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPointShiftForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, FPRHandPoseConditions& HandPoseConditions) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetPointShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static UPlayerHandsConfig* GetPlayerHandsConfig();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static void GetItemShiftForController(const TMap<EPRControllerType, FTransform>& shiftPerController, const EPRControllerType& Controller, FTransform& Shift);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForTagByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, FGameplayTag Tag, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForTag(EPRHandPoseType PoseType, FGameplayTag Tag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForStaticMeshByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UStaticMesh* StaticMesh, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForStaticMesh(EPRHandPoseType PoseType, UStaticMesh* StaticMesh) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, UObject* Object, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    FPRHandPoseInfosPair GetHandsPoseForObject(EPRHandPoseType PoseType, UObject* Object) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetHandShiftForObjectByGeometryTypeAndPoseNumber(EPRHandPoseType PoseType, EPRHandPoseGeometryType GeometryType, EPRHandPoseNumber PoseNumber, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, FPRHandPoseConditions& HandPoseConditions) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetHandShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform) const;
    
    UFUNCTION(BlueprintCallable)
    void GetControllerShifts(const EPRControllerType Controller, FTransform& LeftShift, FTransform& RightShift);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetBestPointShiftForObjectByConditions(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool useGripLimit, const FVector& handRootLocation, const UGripMotionControllerComponent* MotionController, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetBestPointShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetBestHandShiftForObjectByConditions(EPRHandPoseType PoseType, UObject* Object, bool bLeft, const FTransform& CurrentHandTransform, bool bUseArmLengthLimit, const FVector& HandShoulderLocation, const UGripMotionControllerComponent* MotionController, bool& bSuccess, FTransform& PoseRelativeTransform, FTransform& PoseWorldTransform, EPRHandPoseGeometryType& PoseGeometryType, EPRHandPoseNumber& PoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetBestHandShiftForObject(EPRHandPoseType PoseType, UObject* Object, bool Left, const FTransform& handTransform, bool& success, FTransform& Result, FTransform& WorldTransform, EPRHandPoseGeometryType& bestGeometryType, EPRHandPoseNumber& bestPoseNumber) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FPRCleanTransform FromTransform(FTransform Transform);
    

    // Fix for true pure virtual functions not being implemented
};

