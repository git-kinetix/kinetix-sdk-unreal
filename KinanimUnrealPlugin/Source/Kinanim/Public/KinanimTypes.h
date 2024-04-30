// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinanimTypes.generated.h"

UENUM(BlueprintType)
enum class EKeyType : uint8
{
	KT_None		= 0			UMETA(DisplayName = "None"),
	KT_Rotation = 1 << 1	UMETA(DisplayName = "Rotation"),
	KT_Position = 1 << 2	UMETA(DisplayName = "Position"),
	KT_Scale	= 1 << 3	UMETA(DisplayName = "Scale")
};

UENUM(BlueprintType)
enum class EKinanimTransform : uint8
{
	KT_Hips                     = 0		UMETA(DisplayName="Hips"),
	KT_LeftUpperLeg             = 1		UMETA(DisplayName="LeftUpperLeg"),
	KT_RightUpperLeg            = 2		UMETA(DisplayName="RightUpperLeg"),
	KT_LeftLowerLeg             = 3		UMETA(DisplayName="LeftLowerLeg"),
	KT_RightLowerLeg            = 4		UMETA(DisplayName="RightLowerLeg"),
	KT_LeftFoot                 = 5		UMETA(DisplayName="LeftFoot"),
	KT_RightFoot                = 6		UMETA(DisplayName="RightFoot"),
	KT_Spine                    = 7		UMETA(DisplayName="Spine"),
	KT_Chest                    = 8		UMETA(DisplayName="Chest"),
	KT_UpperChest               = 9		UMETA(DisplayName="UpperChest"),
	KT_Neck                     = 10 	UMETA(DisplayName="Neck"),
	KT_Head                     = 11 	UMETA(DisplayName="Head"),
	KT_LeftShoulder             = 12 	UMETA(DisplayName="LeftShoulder"),
	KT_RightShoulder            = 13 	UMETA(DisplayName="RightShoulder"),
	KT_LeftUpperArm             = 14 	UMETA(DisplayName="LeftUpperArm"),
	KT_RightUpperArm            = 15 	UMETA(DisplayName="RightUpperArm"),
	KT_LeftLowerArm             = 16 	UMETA(DisplayName="LeftLowerArm"),
	KT_RightLowerArm            = 17 	UMETA(DisplayName="RightLowerArm"),
	KT_LeftHand                 = 18 	UMETA(DisplayName="LeftHand"),
	KT_RightHand                = 19 	UMETA(DisplayName="RightHand"),
	KT_LeftToes                 = 20 	UMETA(DisplayName="LeftToes"),
	KT_RightToes                = 21 	UMETA(DisplayName="RightToes"),
	KT_LeftEye                  = 22 	UMETA(DisplayName="LeftEye"),
	KT_RightEye                 = 23 	UMETA(DisplayName="RightEye"),
	KT_Jaw                      = 24 	UMETA(DisplayName="Jaw"),
	KT_LeftThumbProximal        = 25 	UMETA(DisplayName="LeftThumbProximal"),
	KT_LeftThumbIntermediate    = 26 	UMETA(DisplayName="LeftThumbIntermediate"),
	KT_LeftThumbDistal          = 27 	UMETA(DisplayName="LeftThumbDistal"),
	KT_LeftIndexProximal        = 28 	UMETA(DisplayName="LeftIndexProximal"),
	KT_LeftIndexIntermediate    = 29 	UMETA(DisplayName="LeftIndexIntermediate"),
	KT_LeftIndexDistal          = 30 	UMETA(DisplayName="LeftIndexDistal"),
	KT_LeftMiddleProximal       = 31 	UMETA(DisplayName="LeftMiddleProximal"),
	KT_LeftMiddleIntermediate   = 32 	UMETA(DisplayName="LeftMiddleIntermediate"),
	KT_LeftMiddleDistal         = 33 	UMETA(DisplayName="LeftMiddleDistal"),
	KT_LeftRingProximal         = 34 	UMETA(DisplayName="LeftRingProximal"),
	KT_LeftRingIntermediate     = 35 	UMETA(DisplayName="LeftRingIntermediate"),
	KT_LeftRingDistal           = 36 	UMETA(DisplayName="LeftRingDistal"),
	KT_LeftLittleProximal       = 37 	UMETA(DisplayName="LeftLittleProximal"),
	KT_LeftLittleIntermediate   = 38 	UMETA(DisplayName="LeftLittleIntermediate"),
	KT_LeftLittleDistal         = 39 	UMETA(DisplayName="LeftLittleDistal"),
	KT_RightThumbProximal       = 40 	UMETA(DisplayName="RightThumbProximal"),
	KT_RightThumbIntermediate   = 41 	UMETA(DisplayName="RightThumbIntermediate"),
	KT_RightThumbDistal         = 42 	UMETA(DisplayName="RightThumbDistal"),
	KT_RightIndexProximal       = 43 	UMETA(DisplayName="RightIndexProximal"),
	KT_RightIndexIntermediate   = 44 	UMETA(DisplayName="RightIndexIntermediate"),
	KT_RightIndexDistal         = 45 	UMETA(DisplayName="RightIndexDistal"),
	KT_RightMiddleProximal      = 46 	UMETA(DisplayName="RightMiddleProximal"),
	KT_RightMiddleIntermediate  = 47 	UMETA(DisplayName="RightMiddleIntermediate"),
	KT_RightMiddleDistal        = 48 	UMETA(DisplayName="RightMiddleDistal"),
	KT_RightRingProximal        = 49 	UMETA(DisplayName="RightRingProximal"),
	KT_RightRingIntermediate    = 50 	UMETA(DisplayName="RightRingIntermediate"),
	KT_RightRingDistal          = 51 	UMETA(DisplayName="RightRingDistal"),
	KT_RightLittleProximal      = 52 	UMETA(DisplayName="RightLittleProximal"),
	KT_RightLittleIntermediate  = 53 	UMETA(DisplayName="RightLittleIntermediate"),
	KT_RightLittleDistal        = 54 	UMETA(DisplayName="RightLittleDistal"),
	KT_Count                   
};

UENUM(BlueprintType)
enum class EKinanimBlendshape: uint8
{
	KB_BrowInnerUp         = 0 	UMETA(DisplayName="BrowInnerUp"),	
	KB_BrowDownLeft        = 1 	UMETA(DisplayName="BrowDownLeft"),	
	KB_BrowDownRight       = 2 	UMETA(DisplayName="BrowDownRight"),	
	KB_BrowOuterUpLeft     = 3 	UMETA(DisplayName="BrowOuterUpLeft"),	
	KB_BrowOuterUpRight    = 4 	UMETA(DisplayName="BrowOuterUpRight"),	
	KB_EyeLookUpLeft       = 5 	UMETA(DisplayName="EyeLookUpLeft"),	
	KB_EyeLookUpRight      = 6 	UMETA(DisplayName="EyeLookUpRight"),	
	KB_EyeLookDownLeft     = 7 	UMETA(DisplayName="EyeLookDownLeft"),	
	KB_EyeLookDownRight    = 8 	UMETA(DisplayName="EyeLookDownRight"),	
	KB_EyeLookInLeft       = 9 	UMETA(DisplayName="EyeLookInLeft"),	
	KB_EyeLookInRight      = 10	UMETA(DisplayName="EyeLookInRight"),	
	KB_EyeLookOutLeft      = 11	UMETA(DisplayName="EyeLookOutLeft"),	
	KB_EyeLookOutRight     = 12	UMETA(DisplayName="EyeLookOutRight"),	
	KB_EyeBlinkLeft        = 13	UMETA(DisplayName="EyeBlinkLeft"),	
	KB_EyeBlinkRight       = 14	UMETA(DisplayName="EyeBlinkRight"),	
	KB_EyeSquintRight      = 15	UMETA(DisplayName="EyeSquintRight"),	
	KB_EyeSquintLeft       = 16	UMETA(DisplayName="EyeSquintLeft"),	
	KB_EyeWideLeft         = 17	UMETA(DisplayName="EyeWideLeft"),	
	KB_EyeWideRight        = 18	UMETA(DisplayName="EyeWideRight"),	
	KB_CheekPuff           = 19	UMETA(DisplayName="CheekPuff"),	
	KB_CheekSquintLeft     = 20	UMETA(DisplayName="CheekSquintLeft"),	
	KB_CheekSquintRight    = 21	UMETA(DisplayName="CheekSquintRight"),	
	KB_NoseSneerLeft       = 22	UMETA(DisplayName="NoseSneerLeft"),	
	KB_NoseSneerRight      = 23	UMETA(DisplayName="NoseSneerRight"),	
	KB_JawOpen             = 24	UMETA(DisplayName="JawOpen"),	
	KB_JawForward          = 25	UMETA(DisplayName="JawForward"),	
	KB_JawLeft             = 26	UMETA(DisplayName="JawLeft"),	
	KB_JawRight            = 27	UMETA(DisplayName="JawRight"),	
	KB_MouthFunnel         = 28	UMETA(DisplayName="MouthFunnel"),	
	KB_MouthPucker         = 29	UMETA(DisplayName="MouthPucker"),	
	KB_MouthLeft           = 30	UMETA(DisplayName="MouthLeft"),	
	KB_MouthRight          = 31	UMETA(DisplayName="MouthRight"),	
	KB_MouthRollUpper      = 32	UMETA(DisplayName="MouthRollUpper"),	
	KB_MouthRollLower      = 33	UMETA(DisplayName="MouthRollLower"),	
	KB_MouthShrugUpper     = 34	UMETA(DisplayName="MouthShrugUpper"),	
	KB_MouthShrugLower     = 35	UMETA(DisplayName="MouthShrugLower"),	
	KB_MouthOpen           = 36	UMETA(DisplayName="MouthOpen"),	
	KB_MouthClose          = 37	UMETA(DisplayName="MouthClose"),	
	KB_MouthSmileLeft      = 38	UMETA(DisplayName="MouthSmileLeft"),	
	KB_MouthSmileRight     = 39	UMETA(DisplayName="MouthSmileRight"),	
	KB_MouthFrownLeft      = 40	UMETA(DisplayName="MouthFrownLeft"),	
	KB_MouthFrownRight     = 41	UMETA(DisplayName="MouthFrownRight"),	
	KB_MouthDimpleLeft     = 42	UMETA(DisplayName="MouthDimpleLeft"),	
	KB_MouthDimpleRight    = 43	UMETA(DisplayName="MouthDimpleRight"),	
	KB_MouthUpperUpLeft    = 44	UMETA(DisplayName="MouthUpperUpLeft"),	
	KB_MouthUpperUpRight   = 45	UMETA(DisplayName="MouthUpperUpRight"),	
	KB_MouthLowerDownLeft  = 46	UMETA(DisplayName="MouthLowerDownLeft"),	
	KB_MouthLowerDownRight = 47	UMETA(DisplayName="MouthLowerDownRight"),	
	KB_MouthPressLeft      = 48	UMETA(DisplayName="MouthPressLeft"),	
	KB_MouthPressRight     = 49	UMETA(DisplayName="MouthPressRight"),	
	KB_MouthStretchLeft    = 50	UMETA(DisplayName="MouthStretchLeft"),	
	KB_MouthStretchRight   = 51	UMETA(DisplayName="MouthStretchRight"),	
	KB_TongueOut           = 52	UMETA(DisplayName="TongueOut"),	
	KB_Count                    		
};

UENUM()
enum class ETransformDeclarationFlag : uint64
{
	TDF_Hips                    = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Hips)						UMETA(DisplayName="Hips"),
	TDF_LeftUpperLeg            = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftUpperLeg)				UMETA(DisplayName="LeftUpperLeg"),
	TDF_RightUpperLeg           = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightUpperLeg)				UMETA(DisplayName="RightUpperLeg"),
	TDF_LeftLowerLeg            = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftLowerLeg)				UMETA(DisplayName="LeftLowerLeg"),
	TDF_RightLowerLeg           = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightLowerLeg)				UMETA(DisplayName="RightLowerLeg"),
	TDF_LeftFoot                = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftFoot)					UMETA(DisplayName="LeftFoot"),
	TDF_RightFoot               = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightFoot)					UMETA(DisplayName="RightFoot"),
	TDF_Spine                   = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Spine)						UMETA(DisplayName="Spine"),
	TDF_Chest                   = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Chest)						UMETA(DisplayName="Chest"),
	TDF_UpperChest              = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_UpperChest)				UMETA(DisplayName="UpperChest"),
	TDF_Neck                    = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Neck)						UMETA(DisplayName="Neck"),
	TDF_Head                    = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Head)						UMETA(DisplayName="Head"),
	TDF_LeftShoulder            = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftShoulder)				UMETA(DisplayName="LeftShoulder"),
	TDF_RightShoulder           = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightShoulder)				UMETA(DisplayName="RightShoulder"),
	TDF_LeftUpperArm            = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftUpperArm)				UMETA(DisplayName="LeftUpperArm"),
	TDF_RightUpperArm           = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightUpperArm)				UMETA(DisplayName="RightUpperArm"),
	TDF_LeftLowerArm            = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftLowerArm)				UMETA(DisplayName="LeftLowerArm"),
	TDF_RightLowerArm           = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightLowerArm)				UMETA(DisplayName="RightLowerArm"),
	TDF_LeftHand                = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftHand)					UMETA(DisplayName="LeftHand"),
	TDF_RightHand               = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightHand)					UMETA(DisplayName="RightHand"),
	TDF_LeftToes                = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftToes)					UMETA(DisplayName="LeftToes"),
	TDF_RightToes               = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightToes)					UMETA(DisplayName="RightToes"),
	TDF_LeftEye                 = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftEye)					UMETA(DisplayName="LeftEye"),
	TDF_RightEye                = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightEye)					UMETA(DisplayName="RightEye"),
	TDF_Jaw                     = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_Jaw)						UMETA(DisplayName="Jaw"),
	TDF_LeftThumbProximal       = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftThumbProximal)			UMETA(DisplayName="LeftThumbProximal"),
	TDF_LeftThumbIntermediate   = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftThumbIntermediate)		UMETA(DisplayName="LeftThumbIntermediate"),
	TDF_LeftThumbDistal         = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftThumbDistal)			UMETA(DisplayName="LeftThumbDistal"),
	TDF_LeftIndexProximal       = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftIndexProximal)			UMETA(DisplayName="LeftIndexProximal"),
	TDF_LeftIndexIntermediate   = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftIndexIntermediate)		UMETA(DisplayName="LeftIndexIntermediate"),
	TDF_LeftIndexDistal         = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftIndexDistal)			UMETA(DisplayName="LeftIndexDistal"),
	TDF_LeftMiddleProximal      = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftMiddleProximal)		UMETA(DisplayName="LeftMiddleProximal"),
	TDF_LeftMiddleIntermediate  = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftMiddleIntermediate)	UMETA(DisplayName="LeftMiddleIntermediate"),
	TDF_LeftMiddleDistal        = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftMiddleDistal)			UMETA(DisplayName="LeftMiddleDistal"),
	TDF_LeftRingProximal        = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftRingProximal)			UMETA(DisplayName="LeftRingProximal"),
	TDF_LeftRingIntermediate    = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftRingIntermediate)		UMETA(DisplayName="LeftRingIntermediate"),
	TDF_LeftRingDistal          = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftRingDistal)			UMETA(DisplayName="LeftRingDistal"),
	TDF_LeftLittleProximal      = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftLittleProximal)		UMETA(DisplayName="LeftLittleProximal"),
	TDF_LeftLittleIntermediate  = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftLittleIntermediate)	UMETA(DisplayName="LeftLittleIntermediate"),
	TDF_LeftLittleDistal        = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_LeftLittleDistal)			UMETA(DisplayName="LeftLittleDistal"),
	TDF_RightThumbProximal      = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightThumbProximal)		UMETA(DisplayName="RightThumbProximal"),
	TDF_RightThumbIntermediate  = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightThumbIntermediate)	UMETA(DisplayName="RightThumbIntermediate"),
	TDF_RightThumbDistal        = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightThumbDistal)			UMETA(DisplayName="RightThumbDistal"),
	TDF_RightIndexProximal      = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightIndexProximal)		UMETA(DisplayName="RightIndexProximal"),
	TDF_RightIndexIntermediate  = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightIndexIntermediate)	UMETA(DisplayName="RightIndexIntermediate"),
	TDF_RightIndexDistal        = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightIndexDistal)			UMETA(DisplayName="RightIndexDistal"),
	TDF_RightMiddleProximal     = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightMiddleProximal)		UMETA(DisplayName="RightMiddleProximal"),
	TDF_RightMiddleIntermediate = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightMiddleIntermediate)	UMETA(DisplayName="RightMiddleIntermediate"),
	TDF_RightMiddleDistal       = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightMiddleDistal)			UMETA(DisplayName="RightMiddleDistal"),
	TDF_RightRingProximal       = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightRingProximal)			UMETA(DisplayName="RightRingProximal"),
	TDF_RightRingIntermediate   = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightRingIntermediate)		UMETA(DisplayName="RightRingIntermediate"),
	TDF_RightRingDistal         = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightRingDistal)			UMETA(DisplayName="RightRingDistal"),
	TDF_RightLittleProximal     = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightLittleProximal)		UMETA(DisplayName="RightLittleProximal"),
	TDF_RightLittleIntermediate = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightLittleIntermediate)	UMETA(DisplayName="RightLittleIntermediate"),
	TDF_RightLittleDistal       = (uint64)1 << static_cast<uint64>(EKinanimTransform::KT_RightLittleDistal)			UMETA(DisplayName="RightLittleDistal"),

	TDF_MaxValue = 0 - 1	UMETA(DisplayName="MAX_VALUE"), //18446744073709551615
};

UENUM()
enum class EBlendshapeDeclarationFlag: uint64
{
	BDF_BrowInnerUp			 = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_BrowInnerUp)			UMETA(DisplayName="BrowInnerUp"),
	BDF_BrowDownLeft         = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_BrowDownLeft)		UMETA(DisplayName="BrowDownLeft"),
	BDF_BrowDownRight        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_BrowDownRight)		UMETA(DisplayName="BrowDownRight"),
	BDF_BrowOuterUpLeft      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_BrowOuterUpLeft)		UMETA(DisplayName="BrowOuterUpLeft"),
	BDF_BrowOuterUpRight     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_BrowOuterUpRight)	UMETA(DisplayName="BrowOuterUpRight"),
	BDF_EyeLookUpLeft        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookUpLeft)		UMETA(DisplayName="EyeLookUpLeft"),
	BDF_EyeLookUpRight       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookUpRight)		UMETA(DisplayName="EyeLookUpRight"),
	BDF_EyeLookDownLeft      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookDownLeft)		UMETA(DisplayName="EyeLookDownLeft"),
	BDF_EyeLookDownRight     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookDownRight)	UMETA(DisplayName="EyeLookDownRight"),
	BDF_EyeLookInLeft        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookInLeft)		UMETA(DisplayName="EyeLookInLeft"),
	BDF_EyeLookInRight       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookInRight)		UMETA(DisplayName="EyeLookInRight"),
	BDF_EyeLookOutLeft       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookOutLeft)		UMETA(DisplayName="EyeLookOutLeft"),
	BDF_EyeLookOutRight      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeLookOutRight)		UMETA(DisplayName="EyeLookOutRight"),
	BDF_EyeBlinkLeft         = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeBlinkLeft)		UMETA(DisplayName="EyeBlinkLeft"),
	BDF_EyeBlinkRight        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeBlinkRight)		UMETA(DisplayName="EyeBlinkRight"),
	BDF_EyeSquintRight       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeSquintRight)		UMETA(DisplayName="EyeSquintRight"),
	BDF_EyeSquintLeft        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeSquintLeft)		UMETA(DisplayName="EyeSquintLeft"),
	BDF_EyeWideLeft          = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeWideLeft)			UMETA(DisplayName="EyeWideLeft"),
	BDF_EyeWideRight         = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_EyeWideRight)		UMETA(DisplayName="EyeWideRight"),
	BDF_CheekPuff            = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_CheekPuff)			UMETA(DisplayName="CheekPuff"),
	BDF_CheekSquintLeft      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_CheekSquintLeft)		UMETA(DisplayName="CheekSquintLeft"),
	BDF_CheekSquintRight     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_CheekSquintRight)	UMETA(DisplayName="CheekSquintRight"),
	BDF_NoseSneerLeft        = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_NoseSneerLeft)		UMETA(DisplayName="NoseSneerLeft"),
	BDF_NoseSneerRight       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_NoseSneerRight)		UMETA(DisplayName="NoseSneerRight"),
	BDF_JawOpen              = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_JawOpen)				UMETA(DisplayName="JawOpen"),
	BDF_JawForward           = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_JawForward)			UMETA(DisplayName="JawForward"),
	BDF_JawLeft              = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_JawLeft)				UMETA(DisplayName="JawLeft"),
	BDF_JawRight             = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_JawRight)			UMETA(DisplayName="JawRight"),
	BDF_MouthFunnel          = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthFunnel)			UMETA(DisplayName="MouthFunnel"),
	BDF_MouthPucker          = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthPucker)			UMETA(DisplayName="MouthPucker"),
	BDF_MouthLeft            = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthLeft)			UMETA(DisplayName="MouthLeft"),
	BDF_MouthRight           = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthRight)			UMETA(DisplayName="MouthRight"),
	BDF_MouthRollUpper       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthRollUpper)		UMETA(DisplayName="MouthRollUpper"),
	BDF_MouthRollLower       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthRollLower)		UMETA(DisplayName="MouthRollLower"),
	BDF_MouthShrugUpper      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthShrugUpper)		UMETA(DisplayName="MouthShrugUpper"),
	BDF_MouthShrugLower      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthShrugLower)		UMETA(DisplayName="MouthShrugLower"),
	BDF_MouthOpen            = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthOpen)			UMETA(DisplayName="MouthOpen"),
	BDF_MouthClose           = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthClose)			UMETA(DisplayName="MouthClose"),
	BDF_MouthSmileLeft       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthSmileLeft)		UMETA(DisplayName="MouthSmileLeft"),
	BDF_MouthSmileRight      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthSmileRight)		UMETA(DisplayName="MouthSmileRight"),
	BDF_MouthFrownLeft       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthFrownLeft)		UMETA(DisplayName="MouthFrownLeft"),
	BDF_MouthFrownRight      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthFrownRight)		UMETA(DisplayName="MouthFrownRight"),
	BDF_MouthDimpleLeft      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthDimpleLeft)		UMETA(DisplayName="MouthDimpleLeft"),
	BDF_MouthDimpleRight     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthDimpleRight)	UMETA(DisplayName="MouthDimpleRight"),
	BDF_MouthUpperUpLeft     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthUpperUpLeft)	UMETA(DisplayName="MouthUpperUpLeft"),
	BDF_MouthUpperUpRight    = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthUpperUpRight)	UMETA(DisplayName="MouthUpperUpRight"),
	BDF_MouthLowerDownLeft   = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthLowerDownLeft)	UMETA(DisplayName="MouthLowerDownLeft"),
	BDF_MouthLowerDownRight  = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthLowerDownRight)	UMETA(DisplayName="MouthLowerDownRight"),
	BDF_MouthPressLeft       = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthPressLeft)		UMETA(DisplayName="MouthPressLeft"),
	BDF_MouthPressRight      = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthPressRight)		UMETA(DisplayName="MouthPressRight"),
	BDF_MouthStretchLeft     = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthStretchLeft)	UMETA(DisplayName="MouthStretchLeft"),
	BDF_MouthStretchRight    = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_MouthStretchRight)	UMETA(DisplayName="MouthStretchRight"),
	BDF_TongueOut            = (uint64)1 << static_cast<uint64>(EKinanimBlendshape::KB_TongueOut)			UMETA(DisplayName="TongueOut"),

	BDF_MaxValue = 0 - 1	UMETA(DisplayName="MAX_VALUE"), //18446744073709551615
};

inline ETransformDeclarationFlag operator|(ETransformDeclarationFlag a, ETransformDeclarationFlag b)
{
	return static_cast<ETransformDeclarationFlag>(static_cast<uint64>(a) | static_cast<uint64>(b));
}

inline EBlendshapeDeclarationFlag operator|(EBlendshapeDeclarationFlag a, EBlendshapeDeclarationFlag b)
{
	return static_cast<EBlendshapeDeclarationFlag>(static_cast<uint64>(a) | static_cast<uint64>(b));
}

inline ETransformDeclarationFlag operator|=(ETransformDeclarationFlag& a, ETransformDeclarationFlag b)
{
	return static_cast<ETransformDeclarationFlag>(a = a | b);
}

inline EBlendshapeDeclarationFlag operator|=(EBlendshapeDeclarationFlag& a, EBlendshapeDeclarationFlag b)
{
	return static_cast<EBlendshapeDeclarationFlag>(a = a | b);
}

USTRUCT(BlueprintType)
struct KINANIM_API FTransformData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector3f   Position;
	UPROPERTY()
	FVector4f   Rotation;
	UPROPERTY()
    FVector3f   Scale;

	UPROPERTY()
	bool	bHasPosition;
	UPROPERTY()
	bool	bHasRotation;
	UPROPERTY()
	bool	bHasScale;
};

USTRUCT()
struct KINANIM_API FFrameData
{
	GENERATED_BODY()

	UPROPERTY()
	ETransformDeclarationFlag TransformDeclarationFlag;

	UPROPERTY()
	TArray<FTransformData> Transforms;

	UPROPERTY()
	EBlendshapeDeclarationFlag BlendshapeDeclarationFlag;

	UPROPERTY()
	TArray<float> Blendshapes;

	FFrameData()
		:TransformDeclarationFlag(ETransformDeclarationFlag::TDF_MaxValue),
		BlendshapeDeclarationFlag(EBlendshapeDeclarationFlag::BDF_MaxValue)
	{
		Transforms.SetNumZeroed(static_cast<uint64>(EKinanimTransform::KT_Count));
		Blendshapes.SetNumZeroed(static_cast<uint64>(EKinanimBlendshape::KB_Count));
	}
};

namespace FrameDataUtils
{
	KINANIM_API FFrameData Clone(const FFrameData& InFrame); 
}
