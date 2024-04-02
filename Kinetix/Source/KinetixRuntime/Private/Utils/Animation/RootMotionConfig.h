// // Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionConfig.generated.h"

USTRUCT(BlueprintType)
struct FRootMotionConfig
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Apply Hips Y Position"))
 bool bApplyHipsYPos = false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Apply Hips X And Y Position"))
 bool bApplyHipsXAndZPos = false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Back To Inital Position"))
 bool bBackToInitialPose = false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Bake Into Position X And Z"))
 bool bBakeIntoPoseXZ = false;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Bake Into Position Y"))
 bool bBakeIntoPoseY = false;

 FRootMotionConfig(
  bool bApplyHipsYPos = false,
  bool bApplyHipsXAndZPos = false,
  bool bBackToInitialPose = false,
  bool bBakeIntoPoseXZ = false,
  bool bBakeIntoPoseY = false)
  : bApplyHipsYPos(bApplyHipsYPos),
    bApplyHipsXAndZPos(bApplyHipsXAndZPos),
    bBackToInitialPose(bBackToInitialPose),
    bBakeIntoPoseXZ(bBakeIntoPoseXZ),
    bBakeIntoPoseY(bBakeIntoPoseY)
 {}
	
};

