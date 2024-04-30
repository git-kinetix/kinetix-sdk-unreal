// Copyright Kinetix. All Rights Reserved.

#include "KinanimWrapper/KinanimBoneCompressionCodec.h"

void UKinanimBoneCompressionCodec::DecompressBone(FAnimSequenceDecompressionContext& DecompContext, int32 TrackIndex,
                                                  FTransform& OutAtom) const
{
	OutAtom.SetLocation(GetTrackLocation(DecompContext, TrackIndex));
	OutAtom.SetRotation(GetTrackRotation(DecompContext, TrackIndex));
	OutAtom.SetScale3D(GetTrackScale(DecompContext, TrackIndex));
}

void UKinanimBoneCompressionCodec::DecompressPose(FAnimSequenceDecompressionContext& DecompContext,
                                                  const BoneTrackArray& RotationPairs,
                                                  const BoneTrackArray& TranslationPairs,
                                                  const BoneTrackArray& ScalePairs,
                                                  TArrayView<FTransform>& OutAtoms) const
{
	for (const BoneTrackPair& BoneTrackPair : RotationPairs)
	{
		OutAtoms[BoneTrackPair.AtomIndex].SetRotation(GetTrackRotation(DecompContext, BoneTrackPair.TrackIndex));
	}

	for (const BoneTrackPair& BoneTrackPair : TranslationPairs)
	{
		OutAtoms[BoneTrackPair.AtomIndex].SetLocation(GetTrackLocation(DecompContext, BoneTrackPair.TrackIndex));
	}

	for (const BoneTrackPair& BoneTrackPair : ScalePairs)
	{
		OutAtoms[BoneTrackPair.AtomIndex].SetScale3D(GetTrackScale(DecompContext, BoneTrackPair.TrackIndex));
	}
}

float UKinanimBoneCompressionCodec::TimeToIndex(float SequenceLength, float RelativePos, int32 NumKeys,
                                                EAnimInterpolationType Interpolation, int32& PosIndex0Out,
                                                int32& PosIndex1Out) const
{
	float Alpha;

	if (NumKeys < 2)
	{
		checkSlow(NumKeys == 1); // check if data is empty for some reason.
		PosIndex0Out = 0;
		PosIndex1Out = 0;
		return 0.0f;
	}
	// Check for before-first-frame case.
	if (RelativePos <= 0.f)
	{
		PosIndex0Out = 0;
		PosIndex1Out = 0;
		Alpha = 0.0f;
	}
	else
	{
		NumKeys -= 1; // never used without the minus one in this case
		// Check for after-last-frame case.
		if (RelativePos >= 1.0f)
		{
			// If we're not looping, key n-1 is the final key.
			PosIndex0Out = NumKeys;
			PosIndex1Out = NumKeys;
			Alpha = 0.0f;
		}
		else
		{
			// For non-looping animation, the last frame is the ending frame, and has no duration.
			const float KeyPos = RelativePos * float(NumKeys);
			checkSlow(KeyPos >= 0.0f);
			const float KeyPosFloor = floorf(KeyPos);
			PosIndex0Out = FMath::Min(FMath::TruncToInt(KeyPosFloor), NumKeys);
			Alpha = (Interpolation == EAnimInterpolationType::Step) ? 0.0f : KeyPos - KeyPosFloor;
			PosIndex1Out = FMath::Min(PosIndex0Out + 1, NumKeys);
		}
	}
	return Alpha;
}

FQuat UKinanimBoneCompressionCodec::GetTrackRotation(FAnimSequenceDecompressionContext& DecompContext,
                                                     const int32 TrackIndex) const
{
	int32 FrameA = 0;
	int32 FrameB = 0;

	float Alpha =
		TimeToIndex(
			DecompContext.GetPlayableLength(),
			DecompContext.GetRelativePosition(),
			Tracks[TrackIndex].RotKeys.Num(),
			DecompContext.Interpolation, FrameA, FrameB);
	return FQuat::Slerp(FQuat(Tracks[TrackIndex].RotKeys[FrameA]), FQuat(Tracks[TrackIndex].RotKeys[FrameB]), Alpha);
}

FVector UKinanimBoneCompressionCodec::GetTrackLocation(FAnimSequenceDecompressionContext& DecompContext,
                                                       const int32 TrackIndex) const
{
	int32 FrameA = 0;
	int32 FrameB = 0;

	float Alpha =
		TimeToIndex(
			DecompContext.GetPlayableLength(),
			DecompContext.GetRelativePosition(),
			Tracks[TrackIndex].PosKeys.Num(),
			DecompContext.Interpolation, FrameA, FrameB);
	return FMath::Lerp(
		FVector(Tracks[TrackIndex].PosKeys[FrameA]),
		FVector(Tracks[TrackIndex].PosKeys[FrameB]), Alpha);
}

FVector UKinanimBoneCompressionCodec::GetTrackScale(FAnimSequenceDecompressionContext& DecompContext,
                                                    const int32 TrackIndex) const
{
	int32 FrameA = 0;
	int32 FrameB = 0;

	float Alpha =
		TimeToIndex(
			DecompContext.GetPlayableLength(),
			DecompContext.GetRelativePosition(),
			Tracks[TrackIndex].ScaleKeys.Num(),
			DecompContext.Interpolation, FrameA, FrameB);

	return FMath::Lerp(
		FVector(Tracks[TrackIndex].ScaleKeys[FrameA]),
		FVector(Tracks[TrackIndex].ScaleKeys[FrameB]), Alpha);

}
