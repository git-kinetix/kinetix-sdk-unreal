// Copyright Kinetix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KinetixCharacterComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Core/Network/KinetixNetworkedPose.h"
#include "Data/KinetixDataLibrary.h"
#include "Interfaces/KinetixSamplerInterface.h"
#include "PoseSearch/PoseSearchAssetSampler.h"
#include "AnimSequenceSamplerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="AnimSequenceSamplerComponent"))
class UAnimSequenceSamplerComponent
	: public UActorComponent,
	  public IKinetixSamplerInterface
{
	GENERATED_BODY()

public:
	UAnimSequenceSamplerComponent(const FObjectInitializer& ObjectInitializer);
	// Sets default values for this component's properties
	UAnimSequenceSamplerComponent(UActorComponent* ActorComponent);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SetAnimatorWasEnabled(const bool Value);
	bool GetAnimInstanceWasEnabled() const;

	void Play(UAnimSequence* InAnimSequence, const FAnimationID& InAnimationID);
	void Play(FAnimationQueue& InAnimQueue);

	void SetDebugPoesable(UPoseableMeshComponent* InPoseableMeshComponent);
	void SetContext(const AActor* InActor);

#pragma region IKinetixSamplerInterface inheritance
	virtual void PlayAnimation_Implementation(UAnimSequence* InAnimSequence) override;
	virtual void StopAnimation_Implementation() override;
#pragma endregion

protected:
	void EnableAnimInstance();
	void DisableAnimInstance();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerSendFramePose(FKinetixNetworkedPose NetworkedPose);

	UFUNCTION(NetMulticast, Unreliable)
	void AllDispatchPose(FKinetixNetworkedPose NetworkedPose);
	
	UFUNCTION()
	void KinetixStartAnimation(const FAnimationID& AnimationID);

private:
	void OnBlendInEnded();
	void OnBlendOutEnded();
	void ComputeBlendTime();

	void ForceBlendOut();

	void NextAnimation();

public:
	TDelegate<void(FAnimationID)> OnAnimationStart;
	TDelegate<void(FAnimationID)> OnAnimationEnd;

	uint64 LastTickFrame;

private:
	UPROPERTY(VisibleAnywhere, Category="Kinetix")
	UPoseableMeshComponent* PoseableMeshComp;

	UAnimSequence* AnimSequenceToPlay;

	FAnimationQueue AnimQueue;

	USkeletalMeshComponent* SkeletalMeshComponentToPause;

	UPROPERTY()
	UKinetixCharacterComponent* KCC;

	float Duration;
	float Time;
	float TimeSinceLastNetUpdate;
	float TimeBetweenNetUpdates;
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

	FAnimExtractContext Context;
	UE::PoseSearch::FSequenceBaseSampler Sampler;

	FBoneContainer RequiredBones;

	TArray<FTransform> BoneSpaceTransforms;
	TArray<FName> BoneNames;

	FKinetixNetworkedPose CachePose;
};
