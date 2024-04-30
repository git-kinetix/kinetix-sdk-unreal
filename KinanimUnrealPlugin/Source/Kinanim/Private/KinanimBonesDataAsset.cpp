// Copyright Kinetix. All Rights Reserved.


#include "KinanimBonesDataAsset.h"

#include "KinanimTypes.h"

UKinanimBonesDataAsset::UKinanimBonesDataAsset()
{
	BonesRef.SetNumZeroed(static_cast<uint8>(EKinanimTransform::KT_Count));
}

FString UKinanimBonesDataAsset::GetBoneNameByIndex(const EKinanimTransform& InIndex) const
{
	switch (InIndex)
	{
	case EKinanimTransform::KT_Hips:
		return Hips;

	case EKinanimTransform::KT_LeftUpperLeg:
		return LeftUpperLeg;

	case EKinanimTransform::KT_RightUpperLeg:
		return RightUpperLeg;

	case EKinanimTransform::KT_LeftLowerLeg:
		return LeftLowerLeg;

	case EKinanimTransform::KT_RightLowerLeg:
		return RightLowerLeg;

	case EKinanimTransform::KT_LeftFoot:
		return LeftFoot;

	case EKinanimTransform::KT_RightFoot:
		return RightFoot;

	case EKinanimTransform::KT_Spine:
		return Spine;

	case EKinanimTransform::KT_Chest:
		return Chest;

	case EKinanimTransform::KT_UpperChest:
		return UpperChest;

	case EKinanimTransform::KT_Neck:
		return Neck;

	case EKinanimTransform::KT_Head:
		return Head;

	case EKinanimTransform::KT_LeftShoulder:
		return LeftShoulder;

	case EKinanimTransform::KT_RightShoulder:
		return RightShoulder;

	case EKinanimTransform::KT_LeftUpperArm:
		return LeftUpperArm;

	case EKinanimTransform::KT_RightUpperArm:
		return RightUpperArm;

	case EKinanimTransform::KT_LeftLowerArm:
		return LeftLowerArm;

	case EKinanimTransform::KT_RightLowerArm:
		return RightLowerArm;

	case EKinanimTransform::KT_LeftHand:
		return LeftHand;

	case EKinanimTransform::KT_RightHand:
		return RightHand;

	case EKinanimTransform::KT_LeftToes:
		return LeftToes;

	case EKinanimTransform::KT_RightToes:
		return RightToes;

	case EKinanimTransform::KT_LeftEye:
		return LeftEye;

	case EKinanimTransform::KT_RightEye:
		return RightEye;

	case EKinanimTransform::KT_Jaw:
		return Jaw;

	case EKinanimTransform::KT_LeftThumbProximal:
		return LeftThumbProximal;

	case EKinanimTransform::KT_LeftThumbIntermediate:
		return LeftThumbIntermediate;

	case EKinanimTransform::KT_LeftThumbDistal:
		return LeftThumbDistal;

	case EKinanimTransform::KT_LeftIndexProximal:
		return LeftIndexProximal;

	case EKinanimTransform::KT_LeftIndexIntermediate:
		return LeftIndexIntermediate;

	case EKinanimTransform::KT_LeftIndexDistal:
		return LeftIndexDistal;

	case EKinanimTransform::KT_LeftMiddleProximal:
		return LeftMiddleProximal;

	case EKinanimTransform::KT_LeftMiddleIntermediate:
		return LeftMiddleIntermediate;

	case EKinanimTransform::KT_LeftMiddleDistal:
		return LeftMiddleDistal;

	case EKinanimTransform::KT_LeftRingProximal:
		return LeftRingProximal;

	case EKinanimTransform::KT_LeftRingIntermediate:
		return LeftRingIntermediate;

	case EKinanimTransform::KT_LeftRingDistal:
		return LeftRingDistal;

	case EKinanimTransform::KT_LeftLittleProximal:
		return LeftLittleProximal;

	case EKinanimTransform::KT_LeftLittleIntermediate:
		return LeftLittleIntermediate;

	case EKinanimTransform::KT_LeftLittleDistal:
		return LeftLittleDistal;

	case EKinanimTransform::KT_RightThumbProximal:
		return RightThumbProximal;

	case EKinanimTransform::KT_RightThumbIntermediate:
		return RightThumbIntermediate;

	case EKinanimTransform::KT_RightThumbDistal:
		return RightThumbDistal;

	case EKinanimTransform::KT_RightIndexProximal:
		return RightIndexProximal;

	case EKinanimTransform::KT_RightIndexIntermediate:
		return RightIndexIntermediate;

	case EKinanimTransform::KT_RightIndexDistal:
		return RightIndexDistal;

	case EKinanimTransform::KT_RightMiddleProximal:
		return RightMiddleProximal;

	case EKinanimTransform::KT_RightMiddleIntermediate:
		return RightMiddleIntermediate;

	case EKinanimTransform::KT_RightMiddleDistal:
		return RightMiddleDistal;

	case EKinanimTransform::KT_RightRingProximal:
		return RightRingProximal;

	case EKinanimTransform::KT_RightRingIntermediate:
		return RightRingIntermediate;

	case EKinanimTransform::KT_RightRingDistal:
		return RightRingDistal;

	case EKinanimTransform::KT_RightLittleProximal:
		return RightLittleProximal;

	case EKinanimTransform::KT_RightLittleIntermediate:
		return RightLittleIntermediate;

	case EKinanimTransform::KT_RightLittleDistal:
		return RightLittleDistal;

	default:
		return FString();
	}
}
