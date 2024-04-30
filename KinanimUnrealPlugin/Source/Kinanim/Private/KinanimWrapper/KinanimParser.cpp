// Copyright Kinetix. All Rights Reserved.

#include "KinanimWrapper/KinanimParser.h"

#include "KinanimTypes.h"
#include "KinanimData.h"
#include "KinanimWrapper.h"

#include "KinanimBoneCompressionCodec.h"
#include "KinanimBonesDataAsset.h"
#include "KinanimCurveCompressionCodec.h"

#include "Kismet/KismetSystemLibrary.h"

FString UKinanimParser::KinanimEnumToSamBone(EKinanimTransform KinanimTransform)
{
	switch (KinanimTransform)
	{
	case EKinanimTransform::KT_Hips:
		return "Hips";
	case EKinanimTransform::KT_LeftUpperLeg:
		return "LeftThigh";
	case EKinanimTransform::KT_RightUpperLeg:
		return "RightThigh";
	case EKinanimTransform::KT_LeftLowerLeg:
		return "LeftShin";
	case EKinanimTransform::KT_RightLowerLeg:
		return "RightShin";
	case EKinanimTransform::KT_LeftFoot:
		return "LeftFoot";
	case EKinanimTransform::KT_RightFoot:
		return "RightFoot";
	case EKinanimTransform::KT_Spine:
		return "Spine1";
	case EKinanimTransform::KT_Chest:
		return "Spine2";
	case EKinanimTransform::KT_UpperChest:
		return "Spine4";
	case EKinanimTransform::KT_Neck:
		return "Neck";
	case EKinanimTransform::KT_Head:
		return "Head";
	case EKinanimTransform::KT_LeftShoulder:
		return "LeftShoulder";
	case EKinanimTransform::KT_RightShoulder:
		return "RightShoulder";
	case EKinanimTransform::KT_LeftUpperArm:
		return "LeftArm";
	case EKinanimTransform::KT_RightUpperArm:
		return "RightArm";
	case EKinanimTransform::KT_LeftLowerArm:
		return "LeftForeArm";
	case EKinanimTransform::KT_RightLowerArm:
		return "RightForeArm";
	case EKinanimTransform::KT_LeftHand:
		return "LeftHand";
	case EKinanimTransform::KT_RightHand:
		return "RightHand";
	case EKinanimTransform::KT_LeftToes:
		return "LeftToe";
	case EKinanimTransform::KT_RightToes:
		return "RightToe";
	case EKinanimTransform::KT_LeftEye:
		return "LeftEye";
	case EKinanimTransform::KT_RightEye:
		return "RightEye";
	case EKinanimTransform::KT_Jaw:
		return "Jaw";
	case EKinanimTransform::KT_LeftThumbProximal:
		return "LeftFinger1Metacarpal";
	case EKinanimTransform::KT_LeftThumbIntermediate:
		return "LeftFinger1Proximal";
	case EKinanimTransform::KT_LeftThumbDistal:
		return "LeftFinger1Distal";
	case EKinanimTransform::KT_LeftIndexProximal:
		return "LeftFinger2Metacarpal";
	case EKinanimTransform::KT_LeftIndexIntermediate:
		return "LeftFinger2Proximal";
	case EKinanimTransform::KT_LeftIndexDistal:
		return "LeftFinger2Distal";
	case EKinanimTransform::KT_LeftMiddleProximal:
		return "LeftFinger3Metacarpal";
	case EKinanimTransform::KT_LeftMiddleIntermediate:
		return "LeftFinger3Proximal";
	case EKinanimTransform::KT_LeftMiddleDistal:
		return "LeftFinger3Distal";
	case EKinanimTransform::KT_LeftRingProximal:
		return "LeftFinger4Metacarpal";
	case EKinanimTransform::KT_LeftRingIntermediate:
		return "LeftFinger4Proximal";
	case EKinanimTransform::KT_LeftRingDistal:
		return "LeftFinger4Distal";
	case EKinanimTransform::KT_LeftLittleProximal:
		return "LeftFinger5Metacarpal";
	case EKinanimTransform::KT_LeftLittleIntermediate:
		return "LeftFinger5Proximal";
	case EKinanimTransform::KT_LeftLittleDistal:
		return "LeftFinger5Distal";
	case EKinanimTransform::KT_RightThumbProximal:
		return "RightFinger1Metacarpal";
	case EKinanimTransform::KT_RightThumbIntermediate:
		return "RightFinger1Proximal";
	case EKinanimTransform::KT_RightThumbDistal:
		return "RightFinger1Distal";
	case EKinanimTransform::KT_RightIndexProximal:
		return "RightFinger2Metacarpal";
	case EKinanimTransform::KT_RightIndexIntermediate:
		return "RightFinger2Proximal";
	case EKinanimTransform::KT_RightIndexDistal:
		return "RightFinger2Distal";
	case EKinanimTransform::KT_RightMiddleProximal:
		return "RightFinger3Metacarpal";
	case EKinanimTransform::KT_RightMiddleIntermediate:
		return "RightFinger3Proximal";
	case EKinanimTransform::KT_RightMiddleDistal:
		return "RightFinger3Distal";
	case EKinanimTransform::KT_RightRingProximal:
		return "RightFinger4Metacarpal";
	case EKinanimTransform::KT_RightRingIntermediate:
		return "RightFinger4Proximal";
	case EKinanimTransform::KT_RightRingDistal:
		return "RightFinger4Distal";
	case EKinanimTransform::KT_RightLittleProximal:
		return "RightFinger5Metacarpal";
	case EKinanimTransform::KT_RightLittleIntermediate:
		return "RightFinger5Proximal";
	case EKinanimTransform::KT_RightLittleDistal:
		return "RightFinger5Distal";
	default:
		return "";
	}
}

FTransform UKinanimParser::ToUnrealTransform(const FTransformData& TrData)
{
	FTransform ToReturn(
		FQuat(
			/* To unity */
			//( TransformData.rotation.x),
			//(-TransformData.rotation.y),
			//(-TransformData.rotation.z),
			//( TransformData.rotation.w)

			-(TrData.Rotation.X),
			-(-TrData.Rotation.Y),
			(-TrData.Rotation.Z),
			(TrData.Rotation.W)
		),

		FVector(
			/* To unity */
			//(-TransformData.position.x),
			//( TransformData.position.y),
			//( TransformData.position.z)

			-(-TrData.Position.X) * 100,
			-(TrData.Position.Y) * 100,
			(TrData.Position.Z) * 100
		),

		TrData.bHasScale
			? FVector(
				/* To unity */
				//TransformData.scale.x,
				//TransformData.scale.y,
				//TransformData.scale.z

				TrData.Scale.X,
				TrData.Scale.Y,
				TrData.Scale.Z
			)
			: FVector(1, 1, 1)
	);

	return ToReturn;
}

UAnimSequence* UKinanimParser::LoadSkeletalAnimationFromStream(USkeletalMesh* SkeletalMesh, void* Stream, const UKinanimBonesDataAsset* InBoneMapping)
{
	//Default scale and matrice
	const float SceneScale = 100;
	FTransform BaseTransform;
	BaseTransform.SetRotation(FRotator(0.f, 180.f, 0.f).Quaternion());
	BaseTransform.SetScale3D(FVector(-1.f, 1.f, 1.f));
	FMatrix SceneBasis = BaseTransform.ToMatrixWithScale();

	//Import kinanim file
	void* Importer = KinanimWrapper::Ctor_KinanimImporter(KinanimWrapper::Ctor_InterpoCompression());
	if (Importer == nullptr)
	{
		UKismetSystemLibrary::PrintString(SkeletalMesh,
		                                  FString::Printf(TEXT("ERROR! Failed to open kinanim stream !")), true,
		                                  true, FLinearColor::Red);
		return nullptr;
	}
	
	KinanimWrapper::KinanimImporter_ReadFile(Importer, Stream);
	
	FKinanimData* data = (FKinanimData*)KinanimWrapper::KinanimImporter_GetResult(Importer);
	if (data == nullptr)
	{
		UKismetSystemLibrary::PrintString(SkeletalMesh,
										  FString::Printf(TEXT("ERROR! Failed to open kinanim stream !")), true,
										  true, FLinearColor::Red);
		return nullptr;
	}

	KinanimWrapper::KinanimImporter_ReleaseResult(Importer);
	KinanimWrapper::Delete_KinanimImporter(Importer);
	Importer = nullptr;

	//Init framecount / frameRate / duration
	int32 NumFrames = data->Header->GetFrameCount();
	if (NumFrames <= 0)
	{
		UKismetSystemLibrary::PrintString(SkeletalMesh,
										  FString::Printf(TEXT("ERROR! kinanim file corrupted, 0 frames found !")), true,
										  true, FLinearColor::Red);
		return nullptr;
	}
	FFrameRate FrameRate(data->Header->frameRate, 1);
	float Duration = NumFrames / data->Header->frameRate;

	//Create sequence
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(GetTransientPackage(), NAME_None, RF_Public);

	//Init with mesh
	AnimSequence->SetSkeleton(SkeletalMesh->GetSkeleton());
	AnimSequence->SetPreviewMesh(SkeletalMesh);

	const TArray<FTransform> BonesPoses = AnimSequence->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose();

	// Use reflection to find the property field related to the AnimSequence's duration
	FFloatProperty* FloatProperty = CastField<FFloatProperty>(UAnimSequence::StaticClass()->FindPropertyByName(TEXT("SequenceLength")));
	FloatProperty->SetPropertyValue_InContainer(AnimSequence, Duration);

	AnimSequence->bEnableRootMotion = false;
	AnimSequence->RootMotionRootLock = ERootMotionRootLock::RefPose;


#if WITH_EDITOR
	AnimSequence->GetController().OpenBracket(FText::FromString("kinanimRuntime"), false);
	AnimSequence->GetController().InitializeModel();
#else
	UKinanimBoneCompressionCodec* CompressionCodec = NewObject<UKinanimBoneCompressionCodec>();
	CompressionCodec->Tracks.AddDefaulted(BonesPoses.Num());
	AnimSequence->CompressedData.CompressedTrackToSkeletonMapTable.AddDefaulted(BonesPoses.Num());
	for (int BoneIndex = 0; BoneIndex < BonesPoses.Num(); ++BoneIndex)
	{
		AnimSequence->CompressedData.CompressedTrackToSkeletonMapTable[BoneIndex] = BoneIndex;
		for (int FrameIndex = 0; FrameIndex < NumFrames; ++FrameIndex)
		{
			CompressionCodec->Tracks[BoneIndex].PosKeys.Add(FVector3f(BonesPoses[BoneIndex].GetLocation()));
			CompressionCodec->Tracks[BoneIndex].RotKeys.Add(FQuat4f(BonesPoses[BoneIndex].GetRotation()));
			CompressionCodec->Tracks[BoneIndex].ScaleKeys.Add(FVector3f(BonesPoses[BoneIndex].GetScale3D()));
		}
	}
	
	AnimSequence->AddToRoot();
#endif

	//Declare tracks
	TMap<FName, TArray<TPair<float, float>>> MorphTargetCurves;

	//Iterate on bones
	for (uint8 i = 0; i < static_cast<uint8>(EKinanimTransform::KT_Count); i++)
	{
		FString TrackName;
		if (!IsValid(InBoneMapping))
		{
			//Enum to sam Bone name
			TrackName = KinanimEnumToSamBone(static_cast<EKinanimTransform>(i));
		}
		else
		{
			TrackName = InBoneMapping->GetBoneNameByIndex(static_cast<EKinanimTransform>(i));
		}
		
		FName BoneName = FName(TrackName);

		FRawAnimSequenceTrack Track = FRawAnimSequenceTrack();

		//Get and check bone index
		int32 BoneIndex = AnimSequence->GetSkeleton()->GetReferenceSkeleton().FindBoneIndex(BoneName);
		if (!AnimSequence->GetSkeleton()->GetReferenceSkeleton().IsValidIndex(BoneIndex))
		{
			UKismetSystemLibrary::PrintString(SkeletalMesh,
			                                  FString::Printf(TEXT("Couldn't find bone '%s' (id: %d)"),
			                                                  *TrackName, static_cast<EKinanimTransform>(i)), true,
			                                  true, FLinearColor::Yellow);
			continue;
		}

		UKismetSystemLibrary::PrintString(SkeletalMesh,
										  FString::Printf(TEXT("Found bone '%s' (index: %i)"),
														  *BoneName.ToString(), BoneIndex), true,
										  true, FLinearColor::Green);
		
		//Get T-Pose bone
		FTransform BoneTransform = BonesPoses[BoneIndex];

		for (int32 j = 0; j < NumFrames; j++)
		{
			FFrameData frame = data->Content->frames[j];
			FTransformData trData = frame.Transforms[i];

			FTransform tr;

			tr = ToUnrealTransform(trData);
			if (i == 0 && j == 100)
			{
				UKismetSystemLibrary::PrintString(SkeletalMesh,
				                                  FString::Printf(TEXT("%s"), *tr.ToHumanReadableString()), true,
				                                  true, FLinearColor::Yellow);
			}


			//All the zombie code are tries to get the rotation and stuff working
			if (trData.bHasRotation)
			{
				Track.RotKeys.Add(FQuat4f(tr.GetRotation()));
			}
			else
			{
				Track.RotKeys.Add(FQuat4f(BoneTransform.GetRotation()));
			}

			if (trData.bHasPosition)
			{
				Track.PosKeys.Add(FVector3f(tr.GetLocation()));
			}
			else
			{
				Track.PosKeys.Add(FVector3f(BoneTransform.GetLocation()));
			}

			if (trData.bHasScale)
			{
				Track.ScaleKeys.Add(FVector3f(tr.GetScale3D()));
			}
			else
			{
				Track.ScaleKeys.Add(FVector3f(BoneTransform.GetScale3D()));
			}
		}

#if WITH_EDITOR
		AnimSequence->GetController().AddBoneCurve(BoneName, false);
		AnimSequence->GetController().SetBoneTrackKeys(BoneName, Track.PosKeys, Track.RotKeys, Track.ScaleKeys, false);
#else
		CompressionCodec->Tracks[BoneIndex] = Track;
#endif
	}

#if WITH_EDITOR
	AnimSequence->GetController().SetFrameRate(FrameRate, false);
	AnimSequence->GetController().SetNumberOfFrames(NumFrames);
	AnimSequence->GetController().NotifyPopulated();
	AnimSequence->GetController().CloseBracket(false);
#else
	AnimSequence->CompressedData.CompressedDataStructure = MakeUnique<FUECompressedAnimData>();
	AnimSequence->CompressedData.CompressedDataStructure->CompressedNumberOfKeys = NumFrames;
	AnimSequence->CompressedData.BoneCompressionCodec = CompressionCodec;
	UKinanimCurveCompressionCodec* AnimCurveCompressionCodec = NewObject<UKinanimCurveCompressionCodec>();
	AnimCurveCompressionCodec->AnimSequence = AnimSequence;
	AnimSequence->CompressedData.CurveCompressionCodec = AnimCurveCompressionCodec;
	AnimSequence->PostLoad();
#endif


	return AnimSequence;
}
