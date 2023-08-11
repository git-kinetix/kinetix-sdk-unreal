// Copyright Kinetix. All Rights Reserved.

#include "Components/AnimSequenceSamplerComponent.h"

#include "KinetixRuntimeModule.h"
#include "Animation/AnimationPoseData.h"
#include "Components/KinetixCharacterComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Core/Network/KinetixNetworkedPose.h"
#include "Kismet/KismetSystemLibrary.h"

UAnimSequenceSamplerComponent::UAnimSequenceSamplerComponent(const FObjectInitializer& ObjectInitializer)
	: LastTickFrame(0), PoseableMeshComp(nullptr), AnimSequenceToPlay(nullptr),
	  SkeletalMeshComponentToPause(nullptr), KCC(nullptr),
	  Duration(0), Time(0), AnimIndex(0),
	  bAnimInstanceWasEnabled(false),
	  BlendState(EBlendState::BS_None),
	  CachedAnimationMode(),
	  BlendTime(0.f)
{
	TimeSinceLastNetUpdate = 0.f;
	TimeBetweenNetUpdates = 0.2f;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

// Sets default values for this component's properties
UAnimSequenceSamplerComponent::UAnimSequenceSamplerComponent(UActorComponent* ActorComponent)
	: LastTickFrame(0), PoseableMeshComp(nullptr), AnimSequenceToPlay(nullptr),
	  SkeletalMeshComponentToPause(nullptr), KCC(nullptr),
	  Duration(0), Time(0), AnimIndex(0),
	  bAnimInstanceWasEnabled(false),
	  BlendState(EBlendState::BS_None),
	  CachedAnimationMode(),
	  BlendTime(0.f), CachePose()
{
	if (!ensure(IsValid(ActorComponent))) return;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UAnimSequenceSamplerComponent::BeginPlay()
{
	Super::BeginPlay();

	auto CurrentOwner = GetOwner();
	if (!IsValid(CurrentOwner))
		return;

	KCC = CurrentOwner->GetComponentByClass<UKinetixCharacterComponent>();
	if (!IsValid(KCC))
	{
		UE_LOG(LogKinetixRuntime,
		       Warning,
		       TEXT(
			       "[AnimSequenceSamplerComponent] BeginPlay: Trying to use AnimSequenceSamplerComponent without KinetixCharacterComponent !"
		       ));
	}

	KCC->RegisterSampler(this);

	SetContext(CurrentOwner);
}

void UAnimSequenceSamplerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(AnimSequenceToPlay))
		return;

	Time += DeltaTime;
	Context.CurrentTime = Time;

	if (AnimSequenceToPlay->GetPlayLength() <= Time)
		return;

	// Needed to avoid crash
	FMemMark Mark(FMemStack::Get());

	// CompactPose is in bone space, so CSPose is here to get it from bone space -> component space
	FCSPose<FCompactPose> CSPose;
	FCompactPose OutPose;

	OutPose.ResetToRefPose(RequiredBones);
	FAnimationPoseData TickPose(OutPose, Curve, Attributes);
	AnimSequenceToPlay->GetAnimationPose(TickPose, Context);
	CSPose.InitPose(MoveTemp(OutPose));

	FBonePoseInfo BoneInfo;

	// ReSharper disable once CppJoinDeclarationAndAssignment
	FTransform BoneTransform;
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		BoneTransform = CSPose.GetComponentSpaceTransform(FCompactPoseBoneIndex(i));
		CachePose.Bones[i].LocalPosition = BoneTransform.GetLocation();
		CachePose.Bones[i].LocalQuaternion = BoneTransform.GetRotation();
		CachePose.Bones[i].LocalScale = BoneTransform.GetScale3D();
	}

	TimeSinceLastNetUpdate += DeltaTime;
	if (TimeSinceLastNetUpdate < TimeBetweenNetUpdates)
		return;

	ServerSendFramePose(CachePose);
	TimeSinceLastNetUpdate = 0.f;
}

void UAnimSequenceSamplerComponent::SetAnimatorWasEnabled(const bool Value)
{
	bAnimInstanceWasEnabled = Value;
}

bool UAnimSequenceSamplerComponent::GetAnimInstanceWasEnabled() const
{
	return bAnimInstanceWasEnabled;
}

void UAnimSequenceSamplerComponent::Play(UAnimSequence* InAnimSequence, const FAnimationID& InAnimationID)
{
	FAnimationQueue NewAnimQueue;
	NewAnimQueue.AnimationIDs.Add(InAnimationID);
	NewAnimQueue.AnimationSequences.Add(InAnimSequence);

	Play(NewAnimQueue);
}

void UAnimSequenceSamplerComponent::Play(FAnimationQueue& InAnimQueue)
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
		// StartAnimation();
		ComputeBlendTime();
	}
}

void UAnimSequenceSamplerComponent::SetDebugPoesable(UPoseableMeshComponent* InPoseableMeshComponent)
{
	PoseableMeshComp = InPoseableMeshComponent;
}

void UAnimSequenceSamplerComponent::SetContext(const AActor* InActor)
{
	if (!IsValid(InActor))
		return;

	SkeletalMeshComponentToPause =
		InActor->GetComponentByClass<USkeletalMeshComponent>();

	if (!ensure(IsValid(SkeletalMeshComponentToPause))) return;

	SkeletalMeshComponentToPause->GetBoneNames(BoneNames);

	UAnimInstance* AnimInstance = SkeletalMeshComponentToPause->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		UE_LOG(LogKinetixRuntime, Warning, TEXT("ERROR: No AnimInstance on %s"),
		       *SkeletalMeshComponentToPause->GetName());
	}

	RequiredBones =
		SkeletalMeshComponentToPause->GetAnimInstance()->GetRequiredBones();

	PoseableMeshComp = InActor->GetComponentByClass<UPoseableMeshComponent>();
	if (GetOwnerRole() == ROLE_AutonomousProxy)
		PoseableMeshComp->SetHiddenInGame(true, true);
}

void UAnimSequenceSamplerComponent::PlayAnimation_Implementation(UAnimSequence* InAnimSequence)
{
	if (!IsValid(InAnimSequence))
		return;

	auto CurrentOwner = SkeletalMeshComponentToPause->GetOwner();
	UKismetSystemLibrary::PrintString(this,
	                                  FString::Printf(TEXT("%s %s PlayAnimation"),
	                                                  *UEnum::GetValueAsString(CurrentOwner->GetLocalRole()),
	                                                  *UEnum::GetValueAsString(CurrentOwner->GetRemoteRole())),
	                                  true, true,
	                                  CurrentOwner->HasAuthority() == true ? FLinearColor::Red : FLinearColor::Blue,
	                                  20.f);

	if (IsValid(AnimSequenceToPlay))
		EnableAnimInstance();


	// if (!IsValid(AnimQueue.AnimationSequences[AnimIndex]))
	// 	return;

	AnimSequenceToPlay = InAnimSequence;

	// float AnimLength = AnimSequenceToPlay->GetPlayLength();

	RequiredBones =
		SkeletalMeshComponentToPause->GetAnimInstance()->GetRequiredBones();

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

	CachePose.Guid = FGuid::NewGuid();
	CachePose.bHasArmature = true;
	CachePose.bHasBlendshapes = false;
	CachePose.bPosEnabled = true;
	CachePose.bScaleEnabled = false;
	CachePose.TimeStamp = FApp::GetCurrentTime();
	CachePose.Bones.SetNumZeroed(RequiredBones.GetNumBones());

	SetComponentTickEnabled(true);

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

void UAnimSequenceSamplerComponent::StopAnimation_Implementation()
{
	IKinetixSamplerInterface::StopAnimation_Implementation();
}

void UAnimSequenceSamplerComponent::EnableAnimInstance()
{
	if (!IsValid(AnimSequenceToPlay))
		return;

	SkeletalMeshComponentToPause->SetAnimationMode(CachedAnimationMode);
	// SkeletalMeshComponentToPause->Play();
}

void UAnimSequenceSamplerComponent::DisableAnimInstance()
{
	CachedAnimationMode = SkeletalMeshComponentToPause->GetAnimationMode();
	SkeletalMeshComponentToPause->Stop();
	SkeletalMeshComponentToPause->SetAnimationMode(EAnimationMode::AnimationCustomMode);
}

bool UAnimSequenceSamplerComponent::ServerSendFramePose_Validate(FKinetixNetworkedPose NetworkedPose)
{
	if (NetworkedPose.Bones.Num() != BoneNames.Num())
	{
		UE_LOG(LogKinetixRuntime,
		       Warning,
		       TEXT(
			       "[AnimSequenceSampler] ServerSendFramePose: Mismatch between local bone mapping (%i bones) and received bone mapping (%i bones)!"
		       ), BoneNames.Num(), NetworkedPose.Bones.Num());
		return false;
	}

	return true;
}

void UAnimSequenceSamplerComponent::ServerSendFramePose_Implementation(FKinetixNetworkedPose NetworkedPose)
{
	if (!IsValid(PoseableMeshComp))
		return;

	AllDispatchPose(NetworkedPose);
}

void UAnimSequenceSamplerComponent::AllDispatchPose_Implementation(FKinetixNetworkedPose NetworkedPose)
{
	if (!IsValid(PoseableMeshComp))
		return;

	FTransform BoneTransform;
	FVector TempVector = FVector::ZeroVector;
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		TempVector = NetworkedPose.Bones[i].LocalPosition.ToFVector();
		BoneTransform.SetLocation(TempVector);
		BoneTransform.SetRotation(NetworkedPose.Bones[i].LocalQuaternion.ToQuat());
		BoneTransform.SetScale3D(NetworkedPose.Bones[i].LocalScale.ToFVector());
		PoseableMeshComp->SetBoneTransformByName(BoneNames[i], BoneTransform, EBoneSpaces::ComponentSpace);
	}
}

void UAnimSequenceSamplerComponent::KinetixStartAnimation(const FAnimationID& AnimationID)
{
}

void UAnimSequenceSamplerComponent::StartAnimationTOCOPY()
{
	if (IsValid(AnimSequenceToPlay))
		EnableAnimInstance();

	if (!IsValid(AnimQueue.AnimationSequences[AnimIndex]))
		return;

	AnimSequenceToPlay = AnimQueue.AnimationSequences[AnimIndex];

	RequiredBones =
		SkeletalMeshComponentToPause->GetAnimInstance()->GetRequiredBones();

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

	CachePose.Guid = FGuid::NewGuid();
	CachePose.bHasArmature = true;
	CachePose.bHasBlendshapes = false;
	CachePose.bPosEnabled = true;
	CachePose.bScaleEnabled = false;
	CachePose.TimeStamp = FApp::GetCurrentTime();
	CachePose.Bones.SetNumZeroed(RequiredBones.GetNumBones());

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

void UAnimSequenceSamplerComponent::OnBlendInEnded()
{
}

void UAnimSequenceSamplerComponent::OnBlendOutEnded()
{
}

void UAnimSequenceSamplerComponent::ComputeBlendTime()
{
}

void UAnimSequenceSamplerComponent::StopAnimationTOCOPY()
{
	if (/*IsValid(BlendAnimations) && */!IsValid(SkeletalMeshComponentToPause)/* || AnimInstanceStatesInfo*/)
		return;

	if (BlendState == EBlendState::BS_None || BlendState == EBlendState::BS_Out)
		return;

	ForceBlendOut();
}

void UAnimSequenceSamplerComponent::ForceBlendOut()
{
}

void UAnimSequenceSamplerComponent::NextAnimation()
{
}
