// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimBoneCompressionCodec.h"
#include "KinanimBoneCompressionCodec.generated.h"


/**
 * 
 */
UCLASS()
class UKinanimBoneCompressionCodec : public UAnimBoneCompressionCodec
{
	GENERATED_BODY()

public:
	virtual void DecompressBone(
		FAnimSequenceDecompressionContext& DecompContext,
		int32 TrackIndex,
		FTransform& OutAtom) const override;

	virtual void DecompressPose(
		FAnimSequenceDecompressionContext& DecompContext,
		const BoneTrackArray& RotationPairs,
		const BoneTrackArray& TranslationPairs,
		const BoneTrackArray& ScalePairs,
		TArrayView<FTransform>& OutAtoms) const override;

	TArray<FRawAnimSequenceTrack> Tracks;

protected:
	float TimeToIndex(
		float SequenceLength,
		float RelativePos,
		int32 NumKeys,
		EAnimInterpolationType Interpolation,
		int32& PosIndex0Out,
		int32& PosIndex1Out) const;

	FQuat GetTrackRotation(
		FAnimSequenceDecompressionContext& DecompContext,
		const int32 TrackIndex) const;
	FVector GetTrackLocation(
		FAnimSequenceDecompressionContext& DecompContext,
		const int32 TrackIndex) const;
	FVector GetTrackScale(
		FAnimSequenceDecompressionContext& DecompContext,
		const int32 TrackIndex) const;
	
};
