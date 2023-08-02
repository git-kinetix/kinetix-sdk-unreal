// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PoseableMeshComponent.h"
#include "Data/KinetixDataLibrary.h"
#include "PoseSearch/PoseSearchAssetSampler.h"

class FAnimSequenceSampler
	: public TSharedFromThis<FAnimSequenceSampler>, public FTickableGameObject
{
public:
	// Sets default values for this component's properties
	FAnimSequenceSampler(UActorComponent* ActorComponent);

	// Needs to be virtual to be TSharedPtr
	virtual ~FAnimSequenceSampler();

#pragma region FTickableGameObject inheritance
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
#pragma endregion FTickableGameObject inheritance

	void SetAnimatorWasEnabled(const bool Value);
	bool GetAnimInstanceWasEnabled() const;

	void Play(UAnimSequence* InAnimSequence, const FAnimationID& InAnimationID);
	void Play(FAnimationQueue& InAnimQueue);
	
	void SetDebugPoesable(UPoseableMeshComponent* InPoseableMeshComponent);

protected:
	void EnableAnimInstance();
	void DisableAnimInstance();

private:
	void StartAnimation();

	void OnBlendInEnded();
	void OnBlendOutEnded();
	void ComputeBlendTime();
	void StopAnimation();
	void ForceBlendOut();

	void NextAnimation();

public:
	TDelegate<void(FAnimationID)> OnAnimationStart;
	TDelegate<void(FAnimationID)> OnAnimationEnd;

	uint64 LastTickFrame;

private:
	UPoseableMeshComponent* PoseableMeshComp;

	UAnimSequence* AnimSequenceToPlay;

	FAnimationQueue AnimQueue;

	USkeletalMeshComponent* SkeletalMeshComponentToPause;

	float Duration;
	float Time;
	int AnimIndex;
	bool bAnimInstanceWasEnabled;

	EBlendState BlendState;

	FAnimationID LastAnimationPlayed;

	EAnimationMode::Type CachedAnimationMode;

	float BlendTime;
	const float BlendInTime = 0.25f;
	const float BlendOutTime = 0.25f;
	const float BlendInterTime = 0.25f;

	FCompactPose CompactPose;
	FBlendedCurve Curve;
	UE::Anim::FStackAttributeContainer Attributes;
	FAnimationPoseData PoseData;
	FAnimExtractContext Context;
	UE::PoseSearch::FSequenceBaseSampler Sampler;

	FBoneContainer RequiredBones;

	TArray<FTransform> BoneSpaceTransforms;
	TArray<FName> BoneNames;
};
