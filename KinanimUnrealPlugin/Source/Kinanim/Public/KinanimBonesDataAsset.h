// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinanimTypes.h"
#include "Engine/DataAsset.h"
#include "KinanimBonesDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class KINANIM_API UKinanimBonesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UKinanimBonesDataAsset();

	FString GetBoneNameByIndex(const EKinanimTransform& InIndex) const;

public:
	UPROPERTY(EditAnywhere)
	TArray<FString> BonesRef;

	UPROPERTY(EditAnywhere)
	FString Hips;
	UPROPERTY(EditAnywhere)
	FString LeftUpperLeg;
	UPROPERTY(EditAnywhere)
	FString RightUpperLeg;
	UPROPERTY(EditAnywhere)
	FString LeftLowerLeg;
	UPROPERTY(EditAnywhere)
	FString RightLowerLeg;
	UPROPERTY(EditAnywhere)
	FString LeftFoot;
	UPROPERTY(EditAnywhere)
	FString RightFoot;
	UPROPERTY(EditAnywhere)
	FString Spine;
	UPROPERTY(EditAnywhere)
	FString Chest;
	UPROPERTY(EditAnywhere)
	FString UpperChest;
	UPROPERTY(EditAnywhere)
	FString Neck;
	UPROPERTY(EditAnywhere)
	FString Head;
	UPROPERTY(EditAnywhere)
	FString LeftShoulder;
	UPROPERTY(EditAnywhere)
	FString RightShoulder;
	UPROPERTY(EditAnywhere)
	FString LeftUpperArm;
	UPROPERTY(EditAnywhere)
	FString RightUpperArm;
	UPROPERTY(EditAnywhere)
	FString LeftLowerArm;
	UPROPERTY(EditAnywhere)
	FString RightLowerArm;
	UPROPERTY(EditAnywhere)
	FString LeftHand;
	UPROPERTY(EditAnywhere)
	FString RightHand;
	UPROPERTY(EditAnywhere)
	FString LeftToes;
	UPROPERTY(EditAnywhere)
	FString RightToes;
	UPROPERTY(EditAnywhere)
	FString LeftEye;
	UPROPERTY(EditAnywhere)
	FString RightEye;
	UPROPERTY(EditAnywhere)
	FString Jaw;
	UPROPERTY(EditAnywhere)
	FString LeftThumbProximal;
	UPROPERTY(EditAnywhere)
	FString LeftThumbIntermediate;
	UPROPERTY(EditAnywhere)
	FString LeftThumbDistal;
	UPROPERTY(EditAnywhere)
	FString LeftIndexProximal;
	UPROPERTY(EditAnywhere)
	FString LeftIndexIntermediate;
	UPROPERTY(EditAnywhere)
	FString LeftIndexDistal;
	UPROPERTY(EditAnywhere)
	FString LeftMiddleProximal;
	UPROPERTY(EditAnywhere)
	FString LeftMiddleIntermediate;
	UPROPERTY(EditAnywhere)
	FString LeftMiddleDistal;
	UPROPERTY(EditAnywhere)
	FString LeftRingProximal;
	UPROPERTY(EditAnywhere)
	FString LeftRingIntermediate;
	UPROPERTY(EditAnywhere)
	FString LeftRingDistal;
	UPROPERTY(EditAnywhere)
	FString LeftLittleProximal;
	UPROPERTY(EditAnywhere)
	FString LeftLittleIntermediate;
	UPROPERTY(EditAnywhere)
	FString LeftLittleDistal;
	UPROPERTY(EditAnywhere)
	FString RightThumbProximal;
	UPROPERTY(EditAnywhere)
	FString RightThumbIntermediate;
	UPROPERTY(EditAnywhere)
	FString RightThumbDistal;
	UPROPERTY(EditAnywhere)
	FString RightIndexProximal;
	UPROPERTY(EditAnywhere)
	FString RightIndexIntermediate;
	UPROPERTY(EditAnywhere)
	FString RightIndexDistal;
	UPROPERTY(EditAnywhere)
	FString RightMiddleProximal;
	UPROPERTY(EditAnywhere)
	FString RightMiddleIntermediate;
	UPROPERTY(EditAnywhere)
	FString RightMiddleDistal;
	UPROPERTY(EditAnywhere)
	FString RightRingProximal;
	UPROPERTY(EditAnywhere)
	FString RightRingIntermediate;
	UPROPERTY(EditAnywhere)
	FString RightRingDistal;
	UPROPERTY(EditAnywhere)
	FString RightLittleProximal;
	UPROPERTY(EditAnywhere)
	FString RightLittleIntermediate;
	UPROPERTY(EditAnywhere)
	FString RightLittleDistal;
};
