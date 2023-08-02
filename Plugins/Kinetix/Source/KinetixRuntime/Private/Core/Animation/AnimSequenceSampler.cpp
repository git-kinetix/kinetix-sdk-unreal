// Copyright Kinetix. All Rights Reserved.

#include "Core/Animation/AnimSequenceSampler.h"

#include "Animation/AnimationPoseData.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
FAnimSequenceSampler::FAnimSequenceSampler(UActorComponent* ActorComponent)
	: LastTickFrame(0), PoseableMeshComp(nullptr), AnimSequenceToPlay(nullptr),
	  SkeletalMeshComponentToPause(nullptr),
	  Duration(0), Time(0), AnimIndex(0),
	  bAnimInstanceWasEnabled(false),
	  BlendState(EBlendState::BS_None),
	  CachedAnimationMode(),
	  BlendTime(0.f),
	  PoseData(CompactPose, Curve, Attributes)
{
	if (!ensure(IsValid(ActorComponent))) return;

	SkeletalMeshComponentToPause =
		ActorComponent->GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	if (!ensure(IsValid(SkeletalMeshComponentToPause))) return;

	SkeletalMeshComponentToPause->GetBoneNames(BoneNames);
	if (BoneNames.Num() != RequiredBones.GetNumBones())
		return;
}

FAnimSequenceSampler::~FAnimSequenceSampler()
{
}

bool FAnimSequenceSampler::IsTickable() const
{
	return !AnimQueue.AnimationIDs.IsEmpty();
}

void FAnimSequenceSampler::Tick(float DeltaTime)
{
	if (LastTickFrame == GFrameCounter)
		return;

	if (!IsValid(AnimSequenceToPlay))
		return;

	Time += DeltaTime;
	Context.CurrentTime = Time;

	if (AnimSequenceToPlay->GetPlayLength() <= Time)
		return;

	UKismetSystemLibrary::PrintString(SkeletalMeshComponentToPause, FString::Printf(TEXT("Tick !")), true, false,
	                                  FLinearColor::Yellow, 0.f);

	// Needed to avoid crash
	FMemMark Mark(FMemStack::Get());

	// CompactPose is in bone space, so CSPose is here to get it from bone space -> component space
	FCSPose<FCompactPose> CSPose;
	FCompactPose OutPose;
	
	OutPose.ResetToRefPose(RequiredBones);
	FAnimationPoseData TickPose(OutPose, Curve, Attributes);
	AnimSequenceToPlay->GetAnimationPose(TickPose, Context);
	CSPose.InitPose(MoveTemp(OutPose));

	// ReSharper disable once CppJoinDeclarationAndAssignment
	FTransform BoneTransform;
	
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		BoneTransform = CSPose.GetComponentSpaceTransform(FCompactPoseBoneIndex(i));
		PoseableMeshComp->SetBoneTransformByName(BoneNames[i], BoneTransform, EBoneSpaces::ComponentSpace);
	}

	LastTickFrame = GFrameCounter;
}

TStatId FAnimSequenceSampler::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(
		FAnimSequenceSamplerClass,
		STATGROUP_Tickables);
}

ETickableTickType FAnimSequenceSampler::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

void FAnimSequenceSampler::SetAnimatorWasEnabled(const bool Value)
{
	bAnimInstanceWasEnabled = Value;
}

bool FAnimSequenceSampler::GetAnimInstanceWasEnabled() const
{
	return bAnimInstanceWasEnabled;
}

void FAnimSequenceSampler::Play(UAnimSequence* InAnimSequence, const FAnimationID& InAnimationID)
{
	FAnimationQueue NewAnimQueue;
	NewAnimQueue.AnimationIDs.Add(InAnimationID);
	NewAnimQueue.AnimationSequences.Add(InAnimSequence);

	Play(NewAnimQueue);
}

void FAnimSequenceSampler::Play(FAnimationQueue& InAnimQueue)
{
	if (!AnimQueue.AnimationIDs.IsEmpty())
	{
		OnAnimationEnd.ExecuteIfBound(AnimQueue.AnimationIDs[AnimIndex]);
	}

	if (InAnimQueue.AnimationSequences.Num() != 0)
	{
		AnimQueue = InAnimQueue;
		AnimIndex = 0;
		Time = 0;
		StartAnimation();
		ComputeBlendTime();
	}
}

void FAnimSequenceSampler::SetDebugPoesable(UPoseableMeshComponent* InPoseableMeshComponent)
{
	PoseableMeshComp = InPoseableMeshComponent;
}

void FAnimSequenceSampler::EnableAnimInstance()
{
	if (!IsValid(AnimSequenceToPlay))
		return;

	SkeletalMeshComponentToPause->SetAnimationMode(CachedAnimationMode);
	// SkeletalMeshComponentToPause->Play();
}

void FAnimSequenceSampler::DisableAnimInstance()
{
	CachedAnimationMode = SkeletalMeshComponentToPause->GetAnimationMode();
	SkeletalMeshComponentToPause->Stop();
	SkeletalMeshComponentToPause->SetAnimationMode(EAnimationMode::AnimationCustomMode);
}

void FAnimSequenceSampler::StartAnimation()
{
	if (IsValid(AnimSequenceToPlay))
		EnableAnimInstance();

	if (!IsValid(AnimQueue.AnimationSequences[AnimIndex]))
		return;

	AnimSequenceToPlay = AnimQueue.AnimationSequences[AnimIndex];

	// float AnimLength = AnimSequenceToPlay->GetPlayLength();
	//
	// RequiredBones =
	// 	SkeletalMeshComponentToPause->GetAnimInstance()->GetRequiredBones();
	//
	// CompactPose.ResetToRefPose(RequiredBones);	

	// Curve.InitFrom(RequiredBones);
	// SkeletalMeshComponentToPause->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	// SkeletalMeshComponentToPause->SetComponentTickEnabled(false);

	// UE::PoseSearch::FSequenceBaseSampler::FInput Input;
	// Input.SequenceBase = AnimSequenceToPlay;
	// Sampler.Init(Input);

	Context.CurrentTime = 0.f;
	for (int i = 0; i < RequiredBones.GetNumBones(); ++i)
	{
		Context.BonesRequired.Add(true);
	}

	// Sampler.ExtractPose(Context, PoseData);

	// if (!IsComponentTickEnabled() || FMath::IsNearlyZero(AnimLength, 0.01f))

	// GetOwnerSkeletalMeshComponent();

	// if (!FMath::IsNearlyZero(Duration, 0.01f))
	// {
	// RootMotionUtils.RevertToOffsets();
	// }
	//
	// DisableAnimInstance();
	//
	// Time = BlendTime;
	// Duration = AnimLength;
	// BlendState = EBlendState::BS_In;
	//
	// // RootMotionUtil.SaveOffsets();
	//
	// float BlendDuration = Duration >= BlendInTime ? BlendInTime : Duration;
	// if (AnimQueue.AnimationIDs.Num() >= AnimIndex
	// 	&& AnimQueue.AnimationIDs[AnimIndex].UUID.IsValid())
	// 	OnAnimationStart.ExecuteIfBound(AnimQueue.AnimationIDs[AnimIndex]);

	// BlendAnimations.BlendAnim(AnimSequenceToPlay, BlendDuration, BlendDuration);
	// BlendAnimations.OnBlendEnded.AddUObject(OnBlendInEnded);
}

void FAnimSequenceSampler::OnBlendInEnded()
{
}

void FAnimSequenceSampler::OnBlendOutEnded()
{
}

void FAnimSequenceSampler::ComputeBlendTime()
{
}

void FAnimSequenceSampler::StopAnimation()
{
	if (/*IsValid(BlendAnimations) && */!IsValid(SkeletalMeshComponentToPause)/* || AnimInstanceStatesInfo*/)
		return;

	if (BlendState == EBlendState::BS_None || BlendState == EBlendState::BS_Out)
		return;

	ForceBlendOut();
}

void FAnimSequenceSampler::ForceBlendOut()
{
}

void FAnimSequenceSampler::NextAnimation()
{
}
